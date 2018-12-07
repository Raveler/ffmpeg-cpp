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
		this->streams = streams;

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
			avformat_free_context(containerContext);
			containerContext = nullptr;
		}
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
	}

	void Muxer::WritePacket(AVPacket* pkt)
	{

		// not opened yet - open lazily now
		if (!opened)
		{
			Open();
			opened = true;
		}

		// Write the compressed frame to the media file.
		int ret = av_interleaved_write_frame(containerContext, pkt);
		if (ret < 0)
		{
			throw FFmpegException("Error while writing frame to output container", ret);
		}
	}

	void Muxer::Open()
	{
		// print debug info about the final file format
		av_dump_format(containerContext, 0, fileName.c_str(), 1);

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
		/* Write the trailer, if any. The trailer must be written before you
		* close the CodecContexts open when you wrote the header; otherwise
		* av_write_trailer() may try to use memory that was freed on
		* av_codec_close(). */
		av_write_trailer(containerContext);

		if (!(containerFormat->flags & AVFMT_NOFILE))
		{
			//Close the output file.
			avio_closep(&containerContext->pb);
		}

		// free the stream
		CleanUp();
	}
}