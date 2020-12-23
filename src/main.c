#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "logging.h"
#include "priority_queue.h"

#define MAX_OUTPUT_LINE_COUNT 100
#define MAX_LINE_LENGTH 1000 + 3 // including special chars '\r', '\n', and '\0'

static const int EXPECTED_ARG_COUNT = 1 + 1;
static const int INPUT_FILE_ARG_INDEX = 1;

static const char *PUNCTUATION_MARKS = ".,;:!?";

void remove_chars_from_str(char *str, const char *chars) {
    for (char *p = str; *p != '\0'; p++)
        if (strchr(chars, *p) == NULL)
            *(str++) = *p;
    *str = '\0';
}

PriorityQueue *get_lexicographically_largest_lines_min_pq_from_file(char *filename) {
    PriorityQueue *result = NULL;
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        LOG_ERROR("Unable to open input file %s\n", filename);
        goto exit_1;
    }
    PriorityQueue *pq = pq_create(MAX_OUTPUT_LINE_COUNT, strcmp);
    if (pq == NULL) goto exit_2;
    char buffer[MAX_LINE_LENGTH];
    while (fgets(buffer, MAX_LINE_LENGTH, file) != NULL) {
        remove_chars_from_str(buffer, PUNCTUATION_MARKS);
        if (pq_size(pq) < MAX_OUTPUT_LINE_COUNT || strcmp(buffer, pq_peek(pq)) > 0) {
            if (pq_size(pq) == MAX_OUTPUT_LINE_COUNT) free(pq_poll(pq));
            if (pq_offer(pq, buffer) != 0) goto exit_3;
        }
    }
    if (ferror(file)) {
        LOG_ERROR("Unable to read form input file %s\n", filename);
        goto exit_3;
    }
    result = pq;
    exit_3:
    if (result != pq) pq_free(pq);
    exit_2:
    fclose(file);
    exit_1:
    return result;
}

int output_lines_in_reverse_order_and_free_pq(PriorityQueue *pq) {
    int exit_code = -1;
    char *output[MAX_OUTPUT_LINE_COUNT];
    size_t output_line_count = pq_size(pq);
    for (size_t i = 1; i <= output_line_count; i++)
        output[output_line_count - i] = pq_poll(pq);
    for (size_t i = 0; i < output_line_count; i++)
        if (fputs(output[i], stdout) == EOF) {
            LOG_ERROR("Unable to print output line to stdout\n");
            goto exit_1;
        }
    exit_code = 0;
    exit_1:
    for (size_t i = 0; i < output_line_count; i++) free(output[i]);
    return exit_code;
}

int main(int argc, char *argv[]) {
    if (argc != EXPECTED_ARG_COUNT) {
        LOG_ERROR("Invalid number of arguments. Expected %d arg(s), but found %d arg(s)\n"
                  "Usage: sorter <input file>\n", EXPECTED_ARG_COUNT - 1, argc - 1);
        return -1;
    }
    PriorityQueue *pq = get_lexicographically_largest_lines_min_pq_from_file(argv[INPUT_FILE_ARG_INDEX]);
    if (pq == NULL) return -1;
    int exit_code = output_lines_in_reverse_order_and_free_pq(pq);
    pq_free(pq);
    return exit_code;
}
