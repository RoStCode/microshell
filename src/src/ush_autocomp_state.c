#include "ush_internal.h"
#include "ush_config.h"
#include "ush_utils.h"
#include "ush_node.h"

#include <string.h>

#if USH_CONFIG_ENABLE_FEATURE_AUTOCOMPLETE == 1

void ush_autocomp_state_recall_suffix(struct ush_object *self)
{
        char *suffix = self->autocomp_input + strlen(self->autocomp_input) - self->autocomp_suffix_len;
        if (strlen(suffix) > 0) {
                ush_write_pointer(self, suffix, USH_STATE_READ_CHAR);
        } else {
                ush_autocomp_prepare_candidates(self);
                ush_write_pointer(self, "\r\n", USH_STATE_AUTOCOMP_CANDIDATES_PRINT);
        }
}

void ush_autocomp_state_prepare(struct ush_object *self)
{
        self->autocomp_input = ush_utils_get_last_arg(self->desc->input_buffer);
        self->state = USH_STATE_AUTOCOMP_CANDIDATES_START;
}

void ush_autocomp_state_candidates_start(struct ush_object *self)
{
        self->autocomp_prev_count = 0;
        self->autocomp_suffix_len = 0;
        ush_autocomp_prepare_candidates(self);
        self->state = USH_STATE_AUTOCOMP_CANDIDATES_COUNT;
}

void ush_autocomp_state_candidates_process(struct ush_object *self)
{
        if (self->process_node == NULL) {
                ush_autocomp_check_for_finish(self);
                return;                        
        }

        if (ush_autocomp_check_for_next(self) != false)
                return;
        
        struct ush_file_descriptor const *file = NULL;
        
        if (ush_autocomp_process_file_prepare(self, &file) != false)
                return;

        ush_autocomp_process_file_index(self, file);
}

void ush_autocomp_state_candidates_finish(struct ush_object *self)
{
        if (self->autocomp_prev_state == USH_STATE_AUTOCOMP_CANDIDATES_PRINT) {
                self->state = USH_STATE_AUTOCOMP_PROMPT;

        } else if (self->autocomp_prev_state == USH_STATE_AUTOCOMP_CANDIDATES_COUNT) {
                switch (self->autocomp_count) {
                case 0:
                        self->state = USH_STATE_READ_CHAR;
                        break;
                case 1: {
                        char *suffix = self->autocomp_input + strlen(self->autocomp_input);
                        strcpy(self->autocomp_input, self->autocomp_candidate_name);
                        self->in_pos = strlen(self->desc->input_buffer);
                        ush_write_pointer(self, suffix, USH_STATE_READ_CHAR);
                        break;
                }
                default:
                        self->autocomp_suffix_len = 0;
                        ush_autocomp_optimize_continue(self);
                        break;
                }

        } else if (self->autocomp_prev_state == USH_STATE_AUTOCOMP_CANDIDATES_OPTIMISE) {
                if (self->autocomp_count < self->autocomp_prev_count) {
                        if (self->in_pos > 0)
                                self->in_pos--;
                        self->desc->input_buffer[self->in_pos] = '\0';
                        self->autocomp_suffix_len--;

                        self->state = USH_STATE_AUTOCOMP_RECALL_SUFFIX;
                        return;
                }

                ush_autocomp_optimize_continue(self);
        } else {
                USH_ASSERT(false);
        }
}

#endif /* USH_CONFIG_ENABLE_FEATURE_AUTOCOMPLETE */