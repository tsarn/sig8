/*
 * Creates a bundle out of files.
 * This is intended to be used automatically from CMake scripts.
 *
 * Usage:
 *   ./bundler BUNDLE_NAME outfile.h ([FILE PATH] [FILE NAME])*
 */

#include <stdio.h>

static int lineLength;
static FILE *file;

static inline void emit(int c)
{
    if (lineLength == 0) {
        putc('\n', file);
        putc('"', file);
    }

    if (c == '\"' || c == '\\') {
        fputc('\\', file);
        fputc(c, file);
        lineLength += 2;
    } else if (c >= 32 && c <= 127) {
        fputc(c, file);
        ++lineLength;
    } else {
        fprintf(file, "\\x%02x", c);
        lineLength += 4;
    }

    if (lineLength > 80) {
        putc('"', file);
        lineLength = 0;
    }
}

static void processFile(const char *name, FILE *src)
{
    for (const char *c = name; *c; ++c) {
        emit((unsigned char)(*c));
    }
    emit(0);

    fseek(src, 0L, SEEK_END);
    int size = ftell(src);
    fseek(src, 0L, SEEK_SET);

    for (int i = 0; i < 4; ++i) {
        emit(0xff & (size >> (i * 8)));
    }

    for (int i = 0; i < size; ++i) {
        char b;
        fread(&b, 1, 1, src);
        emit(b);
    }
}

int main(int argc, char **argv)
{
    if (argc < 3) {
        puts("Invalid usage");
        return 1;
    }

    const char *bundleName = argv[1];
    const char *fileName = argv[2];

    file = fopen(fileName, "w");

    fprintf(file, "#ifndef _BUNDLE_%s_INCLUDED_\n", bundleName);
    fprintf(file, "#define _BUNDLE_%s_INCLUDED_\n\n", bundleName);

    fprintf(file, "#ifdef COMPILE_BUNDLE_DATA\n");
    fprintf(file, "const char %s[] =", bundleName);

    for (int i = 3; i < argc - 1; i += 2) {
        const char *path = argv[i];
        const char *name = argv[i + 1];

        FILE *src = fopen(path, "rb");
        processFile(name, src);
        fclose(src);
    }

    emit(0);

    if (lineLength != 0) {
        putc('"', file);
    }

    fprintf(file, ";\n");

    fprintf(file, "#endif /* COMPILE_BUNDLE_DATA */\n");
    fprintf(file, "\nextern const char *%s;\n", bundleName);

    fprintf(file, "\n#endif /* _BUNDLE_%s_INCLUDED_ */\n", bundleName);
    fclose(file);
    return 0;
}
