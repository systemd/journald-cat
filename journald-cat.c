#include <stdio.h>
#include <jansson.h>
#include <systemd/sd-journal.h>

int parse_json(char *text)
{
    json_t *root;
    json_error_t error;
    void *iter;
    char *strval;
    const char *key;
    json_t *value;
    struct iovec *iov = NULL;
    char *key_value = NULL;
    int idx;
    int num_items;
    int needs_free;

    root = json_loads(text, 0, &error);

    if (!root) {
        fprintf(stderr, "error: on line %d: %s\n", error.line,
            error.text);
        return 1;
    }

    if (json_is_object(root)) {
        printf("is object\n");
    }

    num_items = json_object_size(root);
    iov = malloc(num_items * sizeof(struct iovec));

    idx = 0;
    iter = json_object_iter(root);
    while (iter) {
        needs_free = 0;
        key = json_object_iter_key(iter);
        value = json_object_iter_value(iter);

        if (json_is_string(value)) {
            // cast const char * return to ease free'ing later
            strval = (char *)json_string_value(value);
        } else {
            strval =
                (char *)json_dumps(value,
                           JSON_PRESERVE_ORDER |
                           JSON_ENCODE_ANY);
            needs_free = 1;
        }
        key_value = malloc(strlen(key) + strlen(strval) + 1);
        sprintf(key_value, "%s=%s", key, strval);
        if (needs_free)
            free(strval);
        iov[idx].iov_base = key_value;
        iov[idx].iov_len = strlen(key_value);
        ++idx;
        iter = json_object_iter_next(root, iter);
    }

    // Send the iovector to journald.
    sd_journal_sendv(iov, num_items);

    for (idx = 0; idx < num_items; idx++) {
        free(iov[idx].iov_base);
    }
    free(iov);

    return 0;
}

int main()
{
    char *line;
    int bytes_read;
    size_t nbytes = 32768;

    line = (char *)malloc(nbytes + 1);
    bytes_read = getline(&line, &nbytes, stdin);

    while (bytes_read > 0) {
        parse_json(line);
        bytes_read = getline(&line, &nbytes, stdin);
    }

    free(line);
    return 0;
}
