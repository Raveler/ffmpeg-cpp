#include "Muxer.h"
#include "FFmpegException.h"
#include "OutputStream.h"

#include <string>

using namespace std;

namespace ffmpegcpp
{

	Muxer::Muxer(const char* fileName)
	{
		this->fileName = fileName;

		/* allocate the output media context */
		avformat_alloc_output_context2(&containerContext, NULL, NULL, fileName);
		if (!containerContext)
		{
			printf("WARNING: Could not deduce output format from file extension: using MPEG. as default\n");
			avformat_alloc_output_context2(&containerContext, NULL, "mpeg", fileName);
		}
		if (!containerContext)
		{
			throw FFmpegException("Could not allocate container context for " + this->fileName);
		}

		// the format of the container - not necessarily the same as the fileName suggests, see above
		containerFormat = containerContext->oformat;
	}

	Muxer::~Muxer()
	{
		CleanUp();
	}

	void Muxer::CleanUp()
	{
		if (containerContext != nullptr)
		{
			/* Write the trailer, if any. The trailer must be written before you
			* close the CodecContexts open when you wrote the header; otherwise
			* av_write_trailer() may try to use memory that was freed on
			* av_codec_close(). */
			// If we don't ALWAYS do this, we leak memory!
			av_write_trailer(containerContext);

			if (!(containerFormat->flags & AVFMT_NOFILE))
				/* Close the output file. */
				avio_closep(&containerContext->pb);


			avformat_free_context(containerContext);
			containerContext = nullptr;

			// when the container is closed, the related output streams are closed as well,
			// so we clean those up.
			outputStreams.clear();
		}

		// clean up the queue
		for (int i = 0; i < packetQueue.size(); ++i)
		{
			AVPacket* tmp_pkt = packetQueue[i];
			av_packet_free(&tmp_pkt);
		}
		packetQueue.clear();
	}

	void Muxer::AddOutputStream(OutputStream* outputStream)
	{
		if (opened) throw FFmpegException("You cannot open a new stream after something was written to the muxer");

		// create an internal stream and pass it on
		AVStream* stream = avformat_new_stream(containerContext, NULL);
		if (!stream)
		{
			throw FFmpegException("Could not allocate stream for container " + string(containerContext->oformat->name));
		}

		stream->id = containerContext->nb_streams - 1;

		outputStream->OpenStream(stream, containerContext->oformat->flags);

		outputStreams.push_back(outputStream);
	}

	bool Muxer::IsPrimed()
	{
		if (opened) return true; // we were already opened before - always primed from now on!
		bool allPrimed = true;
		for (int i = 0; i < outputStreams.size(); ++i)
		{
			if (!outputStreams[i]->IsPrimed()) allPrimed = false;
		}

		// we are finally primed - open ourselves before we continue.
		if (allPrimed)
		{
			// if we are all primed
			Open();
			opened = true;
			//printf("After %d cached packets, we can finally open the container\n", packetQueue.size());
		}
		return allPrimed;
	}

	void Muxer::WritePacket(AVPacket* pkt)
	{
		// The muxer needs to buffer all the packets UNTIL all streams are primed
		// at that moment, we can actually open ourselves!
		// Because of this, we need to call PreparePipeline on all input sources BEFORE
		// we start running the actual data through. This pipeline preparation step
		// pushes one frame down the pipeline so that the output can be configured properly.
		if (!opened)
		{
			throw FFmpegException("You cannot submit a packet to the muxer until all output streams are fully primed!");
		}

		// submit this packet
		int ret = av_interleaved_write_frame(containerContext, pkt);
		if (ret < 0)
		{
			throw FFmpegException("Error while writing frame to output container", ret);
		}

		return;

		if (!opened)
		{
			// we CAN open now - all streams are primed and ready to go!
			if (IsPrimed())
			{
				Open();
				opened = true;
				printf("After %d cached packets, we can finally open the container\n", packetQueue.size());

				// flush the queue
				for (int i = 0; i < packetQueue.size(); ++i)
				{
					AVPacket* tmp_pkt = packetQueue[i];

					// Write the compressed frame to the media file.
					int ret = av_interleaved_write_frame(containerContext, tmp_pkt);
					if (ret < 0)
					{
						throw FFmpegException("Error while writing frame to output container", ret);
					}

					av_packet_unref(tmp_pkt);
					av_packet_free(&tmp_pkt);
				}
				packetQueue.clear();
			}

			// not ready - buffer the packet
			else
			{
				AVPacket* tmp_pkt = av_packet_alloc();
				if (!tmp_pkt)
				{
					throw FFmpegException("Failed to allocate packet");
				}
				av_packet_ref(tmp_pkt, pkt);
				packetQueue.push_back(tmp_pkt);
			}
		}

		// we are opened now - write this packet!
		if (opened)
		{
			int ret = av_interleaved_write_frame(containerContext, pkt);
			if (ret < 0)
			{
				throw FFmpegException("Error while writing frame to output container", ret);
			}
		}

	}

	void Muxer::Open()
	{
		// open the output file, if needed
		if (!(containerFormat->flags & AVFMT_NOFILE))
		{
			int ret = avio_open(&containerContext->pb, fileName.c_str(), AVIO_FLAG_WRITE);
			if (ret < 0)
			{
				throw FFmpegException("Could not open file for container " + fileName, ret);
			}
		}

		// Write the stream header, if any.
		int ret = avformat_write_header(containerContext, NULL);
		if (ret < 0)
		{
			throw FFmpegException("Error when writing header to output file " + fileName, ret);
		}
	}

	void Muxer::Close()
	{
		// Make sure we drain all the output streams before we write the first packet.
		// We must be sure to do this because in an extreme case, one entire stream
		// might be queueing all its packets before we are opened, so it might not
		// be draining them at all.
		for (int i = 0; i < outputStreams.size(); ++i)
		{
			outputStreams[i]->DrainPacketQueue();
		}

		// free the stream
		CleanUp();
	}
}