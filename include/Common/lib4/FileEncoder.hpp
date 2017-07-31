/*
ATLAN C++ Library: File Encryption.

ATLAN C++ Library is a library for personal studying.
No licence is annouced.
You can use or change any part of it.
When you using it, inform the author is hoped but not required.
*/
#ifndef __ACL__FILEENCRYPT_HPP__
#define __ACL__FILEENCRYPT_HPP__

#ifndef _WIN32
#include <unistd.h>
#else
#include <io.h>
#endif
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <fstream>
#include <vector>
namespace acl
{

class FileEncoder
{
public:
    bool encodeFile(std::string infileName, std::string outFileName) {
        if (!isFileExist(infileName))
            return false;

        file2file_encode(outFileName.c_str(), infileName.c_str());
        return true;
    }

    bool decodeFile(std::string infileName, std::vector<std::string> &result) {
        if (!isFileExist(infileName))
            return false;

        char *all;
        char *all_bak;
        char *end;
        int length;
        char buff[1024];
        all_bak = all = file2data_decode(&length, infileName.c_str());
        end = all + length;

        while(get_one_line(buff, &all_bak, end) != NULL) {
            if(buff[0] == '\0') {
                result.push_back("");
                continue;
            }
            result.push_back(buff);
        }

        free_decode_memory(all);
        return true;
    }
private:
    bool isFileExist(std::string infileName) {
        std::ifstream ifs(infileName.c_str());
        if (!ifs) {
            return false;
        }
        ifs.close();
        return true;
    }
    void encode_one_time(char *output, const char *input, int length) {
        int i;
        int left = 0;
        int right = length - 1;

        if((length == 0) || (input == NULL) || (output == NULL)) return;

        for(i=0; i<length; ) {
            int k = i + 1;

            output[i] = input[right] + (signed char)1;
            right--;

            if(k < length) {
                output[k] = input[left] + (signed char)1;
                left++;
            }

            i = i + 2;
        }
    }


    void decode_one_time(char *output, const char *input, int length) {
        int i;
        int left = 0;
        int right = length - 1;

        if((length == 0) || (input == NULL) || (output == NULL)) return;

        for(i=0; i<length; ) {
            int k = i + 1;

            output[right] = input[i] - (signed char)1;
            right--;

            if(k < length) {
                output[left] = input[k] - (signed char)1;
                left++;
            }

            i = i + 2;
        }
    }


    void encode(char *output, char *input, int length) {
        encode_one_time(output, input, length);

        encode_one_time(input, output, length);

        encode_one_time(output, input, length);
    }


    void decode(char *output, char *input, int length) {
        decode_one_time(output, input, length);

        decode_one_time(input, output, length);

        decode_one_time(output, input, length);
    }


    void save_to_file(const char *outputfile, const char *data, int length) {
        int fd;

        if((length <= 0) || (outputfile == NULL) || (data == NULL)) return;

        (void)remove(outputfile);

#ifdef _WIN32
        fd = open(outputfile, O_WRONLY | O_CREAT | O_BINARY, S_IREAD | S_IWRITE);
#else
        fd = open(outputfile, O_WRONLY | O_CREAT , S_IREAD | S_IWRITE);
#endif
        if(fd == -1) {
            printf("%s create error\n", outputfile);
            exit(0);
        }

        (void)write(fd, data, (size_t)length);

        (void)close(fd);
    }


    int open_file(int *file_length, const char *inputfile) {
        int fd;
        int length;

#ifdef _WIN32
        fd = open(inputfile, O_RDONLY | O_BINARY);
#else
        fd = open(inputfile, O_RDONLY);
#endif
        if(fd == -1) {
            printf("%s open error\n", inputfile);
            exit(0);
        }

        if((length = (int)lseek(fd, 0L, SEEK_END)) == -1) {
            printf("%s lseek error\n", inputfile);
            exit(0);
        }
        (void)lseek(fd, 0L, 0L);

        *file_length = length;

        return fd;
    }


    void data2file_encode(const char *outputfile, char *input, int length) {
        char *output;

        output = (char *)malloc(sizeof(char)*length+8);
        if(output == NULL) {
            printf("data2file_encode: malloc error\n");
            exit(0);
        }
        output[0] = '\0';

        encode(output, input, length);

        save_to_file(outputfile, output, length);

        free(output);
        output = NULL;
    }
    void data2file_encode(char *input, int length) {
        char *output;

        output = (char *)malloc(sizeof(char)*length+8);
        if(output == NULL) {
            printf("data2file_encode: malloc error\n");
            exit(0);
        }
        output[0] = '\0';

        encode(output, input, length);

        free(output);
        output = NULL;
    }


    void data2file_encode_by_descriptor(FILE *fp, char *input, int length) {
        char *output;

        output = (char *)malloc(sizeof(char)*length+8);
        if(output == NULL) {
            printf("fwrite_for_mkbigram: malloc error\n");
            exit(0);
        }
        output[0] = '\0';

        encode(output, input, length);

        (void)fwrite(output, sizeof(char), (size_t)length, fp);

        free(output);
        output = NULL;
    }


    void data2file_decode(const char *outputfile, char *input, int length) {
        char *output;

        output = (char *)malloc(sizeof(char)*length+8);
        if(output == NULL) {
            printf("data2file_encode: malloc error\n");
            exit(0);
        }
        output[0] = '\0';

        decode(output, input, length);

        save_to_file(outputfile, output, length);

        free(output);
        output = NULL;
    }


    void file2file_encode(const char *outputfile, const char *inputfile) {
        int fd;
        int file_length = 0;

        char *input;

        fd = open_file(&file_length, inputfile);

        input = (char *)malloc(sizeof(char)*file_length+8);
        if(input == NULL) {
            printf("file2file_encode: malloc error\n");
            exit(0);
        }
        input[0] = '\0';

        (void)read(fd, input, (size_t)file_length);
        (void)close(fd);

        data2file_encode(outputfile, input, file_length);

        free(input);
        input = NULL;
    }


    void file2file_decode(const char *outputfile, const char *inputfile) {
        int fd;
        int file_length = 0;

        char *input;

        fd = open_file(&file_length, inputfile);

        input = (char *)malloc(sizeof(char)*file_length+8);
        if(input == NULL) {
            printf("file2file_decode: malloc error\n");
            exit(0);
        }
        input[0] = '\0';

        (void)read(fd, input, (size_t)file_length);
        (void)close(fd);

        data2file_decode(outputfile, input, file_length);

        free(input);
        input = NULL;
    }


    char *file2data_decode(int *length, const char *inputfile) {
        int fd;
        int file_length = 0;

        char *input;
        char *output;

        fd = open_file(&file_length, inputfile);

        input = (char *)malloc(sizeof(char)*file_length+8);
        if(input == NULL) {
            printf("file2data_decode: malloc error\n");
            exit(0);
        }
        //input[0] = '\0';
		memset(input, 0, file_length+8);

        ssize_t nRet = read(fd, input, (size_t)file_length);
        (void)close(fd);

        output = (char *)malloc(sizeof(char)*file_length+8);
        if(output == NULL) {
            printf("file2data_decode: malloc error\n");
            exit(0);
        }
        //output[0] = '\0';
		memset(output, 0, file_length+8);

        decode(output, input, file_length);

        free(input);
        input = NULL;

        *length = file_length;

        return output;
    }


    char *file2data_decode_by_descriptor(int *length, FILE *fp) {
        int file_length = 0;

        char *input;
        char *output;

        if(fseek(fp, 0L, SEEK_END) != 0) {
            printf("decode_ngram_read_bin: fseek error\n");
            exit(0);
        }
        if((file_length = (int)ftell(fp)) == -1) {
            printf("decode_ngram_read_bin: ftell error\n");
            exit(0);
        }
        if(fseek(fp, 0L, SEEK_SET) != 0) {
            printf("decode_ngram_read_bin: fseek error\n");
            exit(0);
        }

        input = (char *)malloc(sizeof(char)*file_length+8);
        if(input == NULL) {
            printf("decode_ngram_read_bin: malloc error\n");
            exit(0);
        }
        input[0] = '\0';

        if(fread(input, sizeof(char), (size_t)file_length, fp) != (size_t)file_length ) {
            printf("decode_ngram_read_bin: fread error\n");
            exit(0);
        }

        output = (char *)malloc(sizeof(char)*file_length+8);
        if(output == NULL) {
            printf("decode_ngram_read_bin: malloc error\n");
            exit(0);
        }
        output[0] = '\0';

        decode(output, input, file_length);

        free(input);
        input = NULL;

        *length = file_length;

        return output;
    }


#ifndef true
#define true (1==1)
#endif
    /*@null@*/
    char *get_one_line(char *buf, char **all_bak, const char *end) {
        char *start;
        char *pos;

        if((buf == NULL) || (all_bak == NULL) || (end == NULL)) return NULL;

        if(((start = *all_bak) == NULL) || (start >= end)) return NULL;

        while(true) {
            char *bak;

            if(start >= end) return NULL;

            bak = pos = strchr(start, '\n');

            if(pos == NULL) return NULL;

            if((pos > start) && (pos[-1] == '\r')) pos--;

            if(pos == start) {
                start = bak + 1;

                continue;
            } else {
                memcpy(buf, start, (size_t)(pos-start));
                buf[pos-start] = '\0';

                start = bak + 1;

                break;
            }
        }

        *all_bak = start;

        return start;
    }


    void free_decode_memory(char *all) {
        free(all);
        all = NULL;
    }
};
}
#endif
