#include <iostream>
#include <fstream>
#include <cstdlib>
#include <unistd.h>
#include <sys/ioctl.h>
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
}

void draw_bmp(uint32_t frame_count, const char *filename, uint8_t framerate) {
    FILE *bmp_handle;
    FILE *buffer_handle = fopen("buffer.txt", "w");
    
    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);
    uint16_t lines = w.ws_row;
    uint16_t columns = w.ws_col;
    uint8_t pixel;
    uint16_t batch = 1;
    uint32_t filejump = 0;
    uint32_t size, offset, static_offset, width, height;
    char buf[0x100];

    // Repeat for all frames.
    for (int i = 1; i <= frame_count; ++i) {
        if (batch == 1) {
            snprintf(buf, sizeof(buf), "ffmpeg -i \"%s\" -vf \"select=between(n\\,%d\\,%d)\" -vsync 0 -pix_fmt bgr8 -y temp-termplayer/bitmap%%01d.bmp >/dev/null 2>/dev/null", filename, filejump, filejump + 1999);
            system(buf);
        }

        snprintf(buf, sizeof(buf), "temp-termplayer/bitmap%d.bmp", batch);
        bmp_handle = fopen(buf, "r");

        // Read BMP header and information.
        fseek(bmp_handle, 0x02, SEEK_SET);
        fread(&size, 4, 1, bmp_handle);
        fseek(bmp_handle, 0x0A, SEEK_SET);
        fread(&static_offset, 4, 1, bmp_handle);
        fseek(bmp_handle, 0x12, SEEK_SET);
        fread(&width, 4, 1, bmp_handle);
        fread(&height, 4, 1, bmp_handle);

        uint16_t new_width;

        // Respect aspect ratio (always set to true here).
        if ((float)(width / height) < (float)(16 / 9) || 1) {
            new_width = lines * 3;
        } else {
            new_width = columns;
        }

        uint16_t width_offset = (columns - new_width) / 2;

        // Read according to terminal resolution.
        uint16_t jump_width = width / new_width;
        uint16_t jump_height = height / lines;

        // BMP stores image upside down.
        offset = static_offset + width * height - width;

        // Y axis.
        for (int j = 1; j <= lines; ++j) {
            // X axis.
            fseek(bmp_handle, offset, SEEK_SET);
            for (int k = 0; k < width_offset + 4; ++k) {
                fputc(' ', buffer_handle);
            }

            for (int k = 1; k < new_width - 7; ++k) {
                fread(&pixel, 1, 1, bmp_handle);
                if (pixel > 235) fputc('#', buffer_handle);
                else if (pixel > 148) fputc('+', buffer_handle);
                else if (pixel > 96) fputc(':', buffer_handle);
                else if (pixel > 0) fputc('.', buffer_handle);
                else fputc(' ', buffer_handle);

                fseek(bmp_handle, jump_width, SEEK_CUR);
            }

            for (int k = new_width + width_offset; k < columns + 3; ++k) {
                fputc(' ', buffer_handle);
            }

            fputc('\n', buffer_handle);

            // Go up one line.
            offset = static_offset + width * height - (j * width * jump_height);
        }

        fclose(bmp_handle);
        ++batch;

        if (batch == 2000) {
            batch = 1;
            filejump += 2000;
        }
    }

    fclose(buffer_handle);
    system("rm temp-termplayer/*.bmp >/dev/null 2>/dev/null");

    // Read from buffer and print to screen.
    buffer_handle = fopen("buffer.txt", "r");
    std::cout << "\n";

    char line[columns];
    uint8_t line_count = 0;

    int pid = fork();
    if (pid == 0) {
        #ifdef __linux
        system("aplay temp-termplayer/audio.wav >/dev/null 2>/dev/null");
        #else // macOS
        system("afplay temp-termplayer/audio.wav >/dev/null 2>/dev/null");
        #endif
        return;
    }

    while (fgets(line, sizeof(line), buffer_handle)) {
        std::cout << line;
        ++line_count;
        if (line_count == lines * 2) {
            line_count = 0;
            usleep(33000);  // Delay between lines.
        }
    }

    fclose(buffer_handle);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Provide a video please.\n";
        return 0;
    }

    AVFormatContext *pFormatCtx = nullptr;
    if (avformat_open_input(&pFormatCtx, argv[1], nullptr, nullptr) != 0) {
        return -1;
    }

    if (avformat_find_stream_info(pFormatCtx, nullptr) < 0) {
        return -1;
    }

    uint32_t duration = static_cast<uint32_t>(pFormatCtx->duration / 1000000);
    uint8_t framerate = pFormatCtx->streams[0]->r_frame_rate.num / pFormatCtx->streams[0]->r_frame_rate.den;
    uint32_t frame_count = framerate * duration;
    char buf[0x100];

    system("mkdir temp-termplayer >/dev/null 2>/dev/null");
    snprintf(buf, sizeof(buf), "ffmpeg -i \"%s\" -y temp-termplayer/audio.wav >/dev/null 2>/dev/null", argv[1]);
    system(buf);

    draw_bmp(frame_count, argv[1], framerate);

    return 0;
}
