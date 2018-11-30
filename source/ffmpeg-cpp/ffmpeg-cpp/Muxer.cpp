#include "Muxer.h"
#include "FFmpegException.h"

#include <string>

using namespace std;

namespace ffmpegcpp
{

	Muxer::Muxer(const char* fileName, vector<OutputStream*> streams)
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

		// create the streams
		for (int i = 0; i < streams.size(); ++i)
		{
			streams[i]->OpenStream(containerContext);
		}

		// open the container
		Open();
	}

	void Muxer::Open()
	{
		// print debug info about the final file format
		av_dump_format(containerContext, 0, fileName.c_str(), 1);

		/* open the output file, if needed */
		if (!(containerFormat->flags & AVFMT_NOFILE))
		{
			int ret = avio_open(&containerContext->pb, fileName.c_str(), AVIO_FLAG_WRITE);
			if (ret < 0)
			{
				throw FFmpegException("Could not open file for container " + fileName, ret);
			}
		}

		/* Write the stream header, if any. */
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
			/* Close the output file. */
			avio_closep(&containerContext->pb);

		/* free the stream */
		avformat_free_context(containerContext);
	}
}