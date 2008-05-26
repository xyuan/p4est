/*
  This file is part of p4est.
  p4est is a C library to manage a parallel collection of quadtrees and/or
  octrees.

  Copyright (C) 2008 Carsten Burstedde, Lucas Wilcox.

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <p4est_base.h>
#include <p4est_options.h>

#include <iniparser.h>

#ifdef P4EST_HAVE_GETOPT_H
#include <getopt.h>
#endif

p4est_options_t    *
p4est_options_new (const char *program_path)
{
  p4est_options_t    *opt;

  opt = P4EST_ALLOC_ZERO (p4est_options_t, 1);
  P4EST_CHECK_ALLOC (opt);

  snprintf (opt->program_path, BUFSIZ, "%s", program_path);
  opt->program_name = basename (opt->program_path);
  opt->option_items = p4est_array_new (sizeof (p4est_option_item_t));

  return opt;
}

void
p4est_options_destroy (p4est_options_t * opt)
{
  int                 i;
  p4est_array_t      *items = opt->option_items;
  int                 count = items->elem_count;
  p4est_option_item_t *item;

  for (i = 0; i < count; ++i) {
    item = p4est_array_index (items, i);
    P4EST_FREE (item->string_value);
  }

  p4est_array_destroy (opt->option_items);
  P4EST_FREE (opt);
}

void
p4est_options_add_switch (p4est_options_t * opt, int opt_char,
                          const char *opt_name,
                          int *variable, const char *help_string)
{
  int                 index;
  p4est_array_t      *items = opt->option_items;
  p4est_option_item_t *item;

  P4EST_ASSERT (opt_char != '\0' || opt_name != NULL);
  P4EST_ASSERT (opt_name == NULL || opt_name[0] != '-');

  index = items->elem_count;
  p4est_array_resize (items, index + 1);
  item = p4est_array_index (items, index);

  item->opt_type = P4EST_OPTION_SWITCH;
  item->opt_char = opt_char;
  item->opt_name = opt_name;
  item->opt_var = variable;
  item->has_arg = 0;
  item->help_string = help_string;
  item->string_value = NULL;

  *variable = 0;
}

void
p4est_options_add_int (p4est_options_t * opt, int opt_char,
                       const char *opt_name,
                       int *variable, int init_value, const char *help_string)
{
  int                 index;
  p4est_array_t      *items = opt->option_items;
  p4est_option_item_t *item;

  P4EST_ASSERT (opt_char != '\0' || opt_name != NULL);
  P4EST_ASSERT (opt_name == NULL || opt_name[0] != '-');

  index = items->elem_count;
  p4est_array_resize (items, index + 1);
  item = p4est_array_index (items, index);

  item->opt_type = P4EST_OPTION_INT;
  item->opt_char = opt_char;
  item->opt_name = opt_name;
  item->opt_var = variable;
  item->has_arg = 1;
  item->help_string = help_string;
  item->string_value = NULL;

  *variable = init_value;
}

void
p4est_options_add_double (p4est_options_t * opt, int opt_char,
                          const char *opt_name,
                          double *variable, double init_value,
                          const char *help_string)
{
  int                 index;
  p4est_array_t      *items = opt->option_items;
  p4est_option_item_t *item;

  P4EST_ASSERT (opt_char != '\0' || opt_name != NULL);
  P4EST_ASSERT (opt_name == NULL || opt_name[0] != '-');

  index = items->elem_count;
  p4est_array_resize (items, index + 1);
  item = p4est_array_index (items, index);

  item->opt_type = P4EST_OPTION_DOUBLE;
  item->opt_char = opt_char;
  item->opt_name = opt_name;
  item->opt_var = variable;
  item->has_arg = 1;
  item->help_string = help_string;
  item->string_value = NULL;

  *variable = init_value;
}

void
p4est_options_add_string (p4est_options_t * opt,
                          int opt_char,
                          const char *opt_name,
                          const char **variable,
                          const char *init_value, const char *help_string)
{
  int                 index;
  p4est_array_t      *items = opt->option_items;
  p4est_option_item_t *item;

  P4EST_ASSERT (opt_char != '\0' || opt_name != NULL);
  P4EST_ASSERT (opt_name == NULL || opt_name[0] != '-');

  index = items->elem_count;
  p4est_array_resize (items, index + 1);
  item = p4est_array_index (items, index);

  item->opt_type = P4EST_OPTION_STRING;
  item->opt_char = opt_char;
  item->opt_name = opt_name;
  item->opt_var = variable;
  item->has_arg = 1;
  item->help_string = help_string;

  /* init_value may be NULL */
  *variable = item->string_value = P4EST_STRDUP (init_value);
}

void
p4est_options_add_inifile (p4est_options_t * opt,
                           int opt_char,
                           const char *opt_name, const char *help_string)
{
  int                 index;
  p4est_array_t      *items = opt->option_items;
  p4est_option_item_t *item;

  P4EST_ASSERT (opt_char != '\0' || opt_name != NULL);
  P4EST_ASSERT (opt_name == NULL || opt_name[0] != '-');

  index = items->elem_count;
  p4est_array_resize (items, index + 1);
  item = p4est_array_index (items, index);

  item->opt_type = P4EST_OPTION_INIFILE;
  item->opt_char = opt_char;
  item->opt_name = opt_name;
  item->opt_var = NULL;
  item->has_arg = 1;
  item->help_string = help_string;
  item->string_value = NULL;
}

void
p4est_options_print_help (p4est_options_t * opt, int include_args,
                          FILE * nout)
{
  int                 i, printed;
  p4est_array_t      *items = opt->option_items;
  int                 count = items->elem_count;
  p4est_option_item_t *item;
  const char         *provide_args;
  const char         *provide_short;
  const char         *provide_long;

  if (nout == NULL)
    return;

  provide_args = include_args ? " <ARGUMENTS>" : "";
  if (count == 0) {
    fprintf (nout, "Usage: %s%s\n", opt->program_name, provide_args);
    return;
  }

  fprintf (nout, "Usage: %s <OPTIONS>%s\n", opt->program_name, provide_args);
  fprintf (nout, "   OPTIONS:\n");
  for (i = 0; i < count; ++i) {
    item = p4est_array_index (items, i);
    switch (item->opt_type) {
    case P4EST_OPTION_SWITCH:
      provide_short = "";
      provide_long = "";
      break;
    case P4EST_OPTION_INT:
      provide_short = " <INT>";
      provide_long = "=<INT>";
      break;
    case P4EST_OPTION_DOUBLE:
      provide_short = " <DOUBLE>";
      provide_long = "=<DOUBLE>";
      break;
    case P4EST_OPTION_STRING:
      provide_short = " <STRING>";
      provide_long = "=<STRING>";
      break;
    case P4EST_OPTION_INIFILE:
      provide_short = " <INIFILE>";
      provide_long = "=<INIFILE>";
      break;
    default:
      P4EST_ASSERT_NOT_REACHED ();
    }
    if (item->opt_char != '\0' && item->opt_name != NULL) {
      printed = fprintf (nout, "      -%c%s | --%s%s",
                         item->opt_char, provide_short,
                         item->opt_name, provide_long);
    }
    else if (item->opt_char != '\0') {
      printed = fprintf (nout, "      -%c%s", item->opt_char, provide_short);
    }
    else if (item->opt_name != NULL) {
      printed = fprintf (nout, "      --%s%s", item->opt_name, provide_long);
    }
    else {
      P4EST_ASSERT_NOT_REACHED ();
    }
    if (item->help_string != NULL) {
      fprintf (nout, "%*s%s\n", P4EST_MAX (1, 48 - printed), "",
               item->help_string);
    }
  }

#if 0
  if (include_args) {
    fprintf (nout, "   ARGUMENTS:\n");
  }
#endif
}

void
p4est_options_print_summary (p4est_options_t * opt, FILE * nout)
{
  int                 i, printed;
  p4est_array_t      *items = opt->option_items;
  int                 count = items->elem_count;
  p4est_option_item_t *item;
  const char         *string_val;

  if (nout == NULL)
    return;

  if (count == 0) {
    fprintf (nout, "No command line options specified\n");
    return;
  }

  fprintf (nout, "Summary:\n   OPTIONS:\n");
  for (i = 0; i < count; ++i) {
    item = p4est_array_index (items, i);
    if (item->opt_type == P4EST_OPTION_INIFILE) {
      continue;
    }
    if (item->opt_name == NULL) {
      printed = fprintf (nout, "      -%c: ", item->opt_char);
    }
    else {
      printed = fprintf (nout, "      %s: ", item->opt_name);
    }
    switch (item->opt_type) {
    case P4EST_OPTION_SWITCH:  /* fall through no break */
    case P4EST_OPTION_INT:
      printed += fprintf (nout, "%d", *(int *) item->opt_var);
      break;
    case P4EST_OPTION_DOUBLE:
      printed += fprintf (nout, "%g", *(double *) item->opt_var);
      break;
    case P4EST_OPTION_STRING:
      string_val = *(const char **) item->opt_var;
      if (string_val == NULL) {
        string_val = "<unspecified>";
      }
      printed += fprintf (nout, "%s", string_val);
      break;
    default:
      P4EST_ASSERT_NOT_REACHED ();
    }
    fprintf (nout, "\n");
  }
}

void
p4est_options_print_arguments (p4est_options_t * opt,
                               int first_arg, int argc, char **argv,
                               FILE * nout)
{
  int                 i;

  if (nout == NULL) {
    return;
  }

  if (first_arg == 0) {
    fprintf (nout, "   ARGUMENTS: none\n");
  }
  else {
    fprintf (nout, "   ARGUMENTS:\n");
    for (i = first_arg; i < argc; ++i) {
      fprintf (nout, "      %d: %s\n", i - first_arg, argv[i]);
    }
  }
}

int
p4est_options_load (p4est_options_t * opt, const char *inifile, FILE * nerr)
{
  int                 i;
  p4est_array_t      *items = opt->option_items;
  int                 count = items->elem_count;
  p4est_option_item_t *item;
  dictionary         *dict;
  int                *ivalue;
  double             *dvalue;
  const char         *s, *key;
  char                skey[BUFSIZ], lkey[BUFSIZ];
  int                 found_short, found_long;

  dict = iniparser_load (inifile, nerr);
  if (dict == NULL) {
    return -1;
  }

  for (i = 0; i < count; ++i) {
    item = p4est_array_index (items, i);
    if (item->opt_type == P4EST_OPTION_INIFILE) {
      continue;
    }

    key = NULL;
    skey[0] = lkey[0] = '\0';
    if (item->opt_char != '\0') {
      snprintf (skey, BUFSIZ, "Options:-%c", item->opt_char);
      found_short = iniparser_find_entry (dict, skey);
    }
    if (item->opt_name != NULL) {
      snprintf (lkey, BUFSIZ, "Options:%s", item->opt_name);
      found_long = iniparser_find_entry (dict, lkey);
    }
    if (found_short && found_long) {
      if (nerr != NULL) {
        fprintf (nerr, "Duplicates %s %s in file: %s\n", skey, lkey, inifile);
      }
      iniparser_freedict (dict);
      return -1;
    }
    else if (found_long) {
      key = lkey;
    }
    else if (found_short) {
      key = skey;
    }
    else {
      continue;
    }

    switch (item->opt_type) {
    case P4EST_OPTION_SWITCH:
      *(int *) item->opt_var += iniparser_getboolean (dict, key, 0);
      break;
    case P4EST_OPTION_INT:
      ivalue = (int *) item->opt_var;
      *ivalue = iniparser_getint (dict, key, *ivalue);
      break;
    case P4EST_OPTION_DOUBLE:
      dvalue = (double *) item->opt_var;
      *dvalue = iniparser_getdouble (dict, key, *dvalue);
      break;
    case P4EST_OPTION_STRING:
      s = iniparser_getstring (dict, key, NULL);
      if (s != NULL) {
        P4EST_FREE (item->string_value);        /* deals with NULL */
        *(const char **) item->opt_var = item->string_value =
          P4EST_STRDUP (s);
      }
      break;
    default:
      P4EST_ASSERT_NOT_REACHED ();
    }
  }

  iniparser_freedict (dict);
  return 0;
}

int
p4est_options_parse (p4est_options_t * opt, int argc, char **argv,
                     FILE * nerr)
{
  int                 i, retval;
  int                 position, printed;
  int                 c, option_index;
  int                 item_index;
  p4est_array_t      *items = opt->option_items;
  int                 count = items->elem_count;
  p4est_option_item_t *item;
  char                optstring[BUFSIZ];
  struct option      *longopts, *lo;

  /* build getopt string and long option structures */

  longopts = P4EST_ALLOC_ZERO (struct option, count + 1);
  P4EST_CHECK_ALLOC (longopts);
  lo = longopts;
  position = 0;
  for (i = 0; i < count; ++i) {
    item = p4est_array_index (items, i);
    if (item->opt_char != '\0') {
      printed = snprintf (optstring + position, BUFSIZ - position,
                          "%c%s", item->opt_char, item->has_arg ? ":" : "");
      P4EST_ASSERT (printed > 0);
      position += printed;
    }
    if (item->opt_name != NULL) {
      lo->name = item->opt_name;
      lo->has_arg = item->has_arg;
      lo->flag = &item_index;
      lo->val = i;
      ++lo;
    }
  }

  /* run getopt loop */

  retval = 0;
  opterr = 0;
  while (retval == 0) {
    c = getopt_long (argc, argv, optstring, longopts, &option_index);
    if (c == -1) {
      break;
    }
    if (c == '?') {             /* invalid option */
      if (nerr != NULL) {
        fprintf (nerr, "Encountered invalid option\n");
      }
      retval = -1;
      break;
    }

    item = NULL;
    if (c == 0) {               /* long option */
      item = p4est_array_index (items, item_index);
    }
    else {                      /* short option */
      for (i = 0; i < count; ++i) {
        item = p4est_array_index (items, i);
        if (item->opt_char == c) {
          break;
        }
      }
      if (i == count) {
        if (nerr != NULL) {
          fprintf (nerr, "Encountered invalid short option: %c\n", c);
        }
        retval = -1;
        break;
      }
    }
    switch (item->opt_type) {
    case P4EST_OPTION_SWITCH:
      ++*(int *) item->opt_var;
      break;
    case P4EST_OPTION_INT:
      *(int *) item->opt_var = strtol (optarg, NULL, 0);
      break;
    case P4EST_OPTION_DOUBLE:
      *(double *) item->opt_var = strtod (optarg, NULL);
      break;
    case P4EST_OPTION_STRING:
      P4EST_FREE (item->string_value);  /* deals with NULL */
      *(const char **) item->opt_var = item->string_value =
        P4EST_STRDUP (optarg);
      break;
    case P4EST_OPTION_INIFILE:
      if (p4est_options_load (opt, optarg, nerr)) {
        if (nerr != NULL) {
          fprintf (nerr, "Error loading file: %s\n", optarg);
        }
        retval = -1;            /* this ends option processing */
      }
      break;
    default:
      P4EST_ASSERT_NOT_REACHED ();
    }
  }

  /* free memory and return */
  P4EST_FREE (longopts);
  return retval < 0 ? -1 : optind;
}

/* EOF p4est_options.c */