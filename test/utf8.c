#include "test.h"

/*
 * g_utf16_to_utf8
 */

glong
compare_strings_utf8_pos (const gchar *expected, const gchar *actual, glong size)
{
	int i;
	for (i = 0; i < size; i++)
		if (expected [i] != actual [i])
			return i;
	return -1;
}

RESULT
compare_strings_utf8_RESULT (const gchar *expected, const gchar *actual, glong size)
{
	glong ret;

	ret = compare_strings_utf8_pos (expected, actual, size);
	if (ret < 0)
		return OK;
	return FAILED ("Incorrect output: expected '%s' but was '%s', differ at %d\n", expected, actual, ret);
}

void
gchar_to_gunichar2 (gunichar2 ret[], const gchar *src)
{
	int i;

	for (i = 0; src [i]; i++)
		ret [i] = src [i];
	ret [i] = 0;
}

RESULT
compare_utf16_to_utf8_explicit (const gchar *expected, const gunichar2 *utf16, glong len_in, glong len_out, glong size_spec)
{
	GError *error;
	gchar* ret;
	RESULT result;
	glong in_read, out_read;

	result = NULL;

	error = NULL;
	ret = g_utf16_to_utf8 (utf16, size_spec, &in_read, &out_read, &error);
	if (error) {
		result = FAILED ("The error is %d %s\n", (error)->code, (error)->message);
		g_error_free (error);
		if (ret)
			g_free (ret);
		return result;
	}
	if (in_read != len_in)
		result = FAILED ("Read size is incorrect: expected %d but was %d\n", len_in, in_read);
	else if (out_read != len_out)
		result = FAILED ("Converted size is incorrect: expected %d but was %d\n", len_out, out_read);
	else
		result = compare_strings_utf8_RESULT (expected, ret, len_out);

	g_free (ret);
	if (result)
		return result;

	return OK;
}

RESULT
compare_utf16_to_utf8 (const gchar *expected, const gunichar2 *utf16, glong len_in, glong len_out)
{
	RESULT result;

	result = compare_utf16_to_utf8_explicit (expected, utf16, len_in, len_out, -1);
	if (result != OK)
		return result;
	return compare_utf16_to_utf8_explicit (expected, utf16, len_in, len_out, len_in);
}

RESULT
test_utf16_to_utf8 ()
{
	const gchar *src0 = "", *src1 = "ABCDE", *src2 = "\xE5\xB9\xB4\x27";
	gunichar2 str0 [] = {0}, str1 [6], str2 [] = {0x5E74, 39, 0};
	RESULT result;

	gchar_to_gunichar2 (str1, src1);

	/* empty string */
	result = compare_utf16_to_utf8 (src0, str0, 0, 0);
	if (result != OK)
		return result;

	result = compare_utf16_to_utf8 (src1, str1, 5, 5);
	if (result != OK)
		return result;
	result = compare_utf16_to_utf8 (src2, str2, 2, 4);
	if (result != OK)
		return result;

	return OK;
}

/*
 * g_utf8_to_utf16 
 */

glong
compare_strings_utf16_pos (const gunichar2 *expected, const gunichar2 *actual, glong size)
{
	int i;
	for (i = 0; i < size; i++)
		if (expected [i] != actual [i])
			return i;
	return -1;
}

RESULT
compare_strings_utf16_RESULT (const gunichar2 *expected, const gunichar2 *actual, glong size)
{
	glong ret;

	ret = compare_strings_utf16_pos (expected, actual, size);
	if (ret < 0)
		return OK;
	return FAILED ("Incorrect output: expected '%s' but was '%s'\n", expected, actual);
}

RESULT
compare_utf8_to_utf16_explicit (const gunichar2 *expected, const gchar *utf8, glong len_in, glong len_out, glong size_spec)
{
	GError *error;
	gunichar2* ret;
	RESULT result;
	glong in_read, out_read;

	result = NULL;

	error = NULL;
	ret = g_utf8_to_utf16 (utf8, size_spec, &in_read, &out_read, &error);
	if (error) {
		result = FAILED ("The error is %d %s\n", (error)->code, (error)->message);
		g_error_free (error);
		if (ret)
			g_free (ret);
		return result;
	}
	if (in_read != len_in)
		result = FAILED ("Read size is incorrect: expected %d but was %d\n", len_in, in_read);
	else if (out_read != len_out)
		result = FAILED ("Converted size is incorrect: expected %d but was %d\n", len_out, out_read);
	else
		result = compare_strings_utf16_RESULT (expected, ret, len_out);

	g_free (ret);
	if (result)
		return result;

	return OK;
}


RESULT
compare_utf8_to_utf16 (const gunichar2 *expected, const gchar *utf8, glong len_in, glong len_out)
{
	RESULT result;

	result = compare_utf8_to_utf16_explicit (expected, utf8, len_in, len_out, -1);
	if (result != OK)
		return result;
	return compare_utf8_to_utf16_explicit (expected, utf8, len_in, len_out, len_in);
}

RESULT
test_utf8_seq ()
{
	const gchar *src = "\xE5\xB9\xB4\x27";
	glong in_read, out_read;
	//gunichar2 expected [6];
	GError *error = NULL;
	gunichar2 *dst;

	printf ("got: %s\n", src);
	dst = g_utf8_to_utf16 (src, (glong)strlen (src), &in_read, &out_read, &error);
	if (error != NULL){
		return error->message;
	}

	if (in_read != 4) {
		return FAILED ("in_read is expected to be 4 but was %d\n", in_read);
	}
	if (out_read != 2) {
		return FAILED ("out_read is expected to be 2 but was %d\n", out_read);
	}

	return OK;
}

RESULT
test_utf8_to_utf16 ()
{
	const gchar *src0 = "", *src1 = "ABCDE", *src2 = "\xE5\xB9\xB4\x27";
	gunichar2 str0 [] = {0}, str1 [6], str2 [] = {0x5E74, 39, 0};
	RESULT result;

	gchar_to_gunichar2 (str1, src1);

	/* empty string */
	result = compare_utf8_to_utf16 (str0, src0, 0, 0);
	if (result != OK)
		return result;

	result = compare_utf8_to_utf16 (str1, src1, 5, 5);
	if (result != OK)
		return result;
	result = compare_utf8_to_utf16 (str2, src2, 4, 2);
	if (result != OK)
		return result;

	return OK;
}

RESULT
test_convert ()
{
	gsize n;
	char *s = g_convert ("\242\241\243\242\241\243\242\241\243\242\241\243", -1, "UTF-8", "ISO-8859-1", NULL, &n, NULL);
	guchar *u = (guchar *) s;
	
	if (!s)
		return FAILED ("Expected 24 bytes, got: NULL");

	if (strlen (s) != 24)
		return FAILED ("Expected 24 bytes, got: %d", strlen (s));

	if (u [1] != 162 || u [2] != 194 ||
	    u [3] != 161 || u [4] != 194 ||
	    u [5] != 163 || u [6] != 194)
		return FAILED ("Incorrect conversion");
	
	g_free (s);
	
	return OK;
}


RESULT
test_xdigit ()
{
	static char test_chars[] = {
		'0', '1', '2', '3', '4', 
		'5', '6', '7', '8', '9', 
		'a', 'b', 'c', 'd', 'e', 'f', 'g',
		'A', 'B', 'C', 'D', 'E', 'F', 'G'};
	static gint32 test_values[] = {
		0, 1, 2, 3, 4, 
		5, 6, 7, 8, 9, 
		10, 11, 12, 13, 14, 15, -1,
		10, 11, 12, 13, 14, 15, -1};

		int i =0;

		for (i = 0; i < sizeof(test_chars); i++)
			if (g_unichar_xdigit_value ((gunichar)test_chars[i]) != test_values[i])
				return FAILED("Incorrect value %d at index %d", test_values[i], i);

		return OK;
}

static RESULT
ucs4_to_utf16_check_result (const gunichar2 *result_str, const gunichar2 *expected_str,
			    glong result_items_read, glong expected_items_read,
			    glong result_items_written, glong expected_items_written,
			    GError* result_error, gboolean expect_error)
{
	glong i;
	if (result_items_read != expected_items_read)
		return FAILED("Incorrect number of items read %d", result_items_read);
	if (result_items_written != expected_items_written)
		return FAILED("Incorrect number of items written %d", result_items_written);
	if (result_error && !expect_error)
		return FAILED("There should not be an error code.");
	if (!result_error && expect_error)
		return FAILED("Unexpected error object.");
	if (expect_error && result_str)
		return FAILED("NULL should be returned when an error occurs.");
	if (!expect_error && !result_str)
		return FAILED("When no error occurs NULL should not be returned.");
	for (i=0; i<expected_items_written;i++) {
		if (result_str [i] != expected_str [i])
			return FAILED("Incorrect value %d at index %d", result_str [i], i);
	}
	if (result_str && result_str[expected_items_written] != '\0') 
		return FAILED("Null termination not found at the end of the string.");
	
	return OK;
}

RESULT
test_ucs4_to_utf16 ()
{
	static gunichar str1[12] = {'H','e','l','l','o',' ','W','o','r','l','d','\0'};
	static gunichar2 exp1[12] = {'H','e','l','l','o',' ','W','o','r','l','d','\0'};
	static gunichar str2[3] = {'h',0x80000000,'\0'};
	static gunichar2 exp2[2] = {'h','\0'};
	static gunichar str3[3] = {'h',0xDA00,'\0'};
	static gunichar str4[3] = {'h',0x10FFFF,'\0'};
	static gunichar2 exp4[4] = {'h',0xdbff,0xdfff,'\0'};
	static gunichar str5[7] = {0xD7FF,0xD800,0xDFFF,0xE000,0x110000,0x10FFFF,'\0'};
	static gunichar2 exp5[5] = {0xD7FF,0xE000,0xdbff,0xdfff,'\0'};
	static glong read_write[12] = {1,1,0,0,0,0,1,1,0,0,1,2};
	gunichar2* res;
	glong items_read, items_written, current_write_index;
	GError* err=0;
	RESULT check_result;
	glong i;
	
	res = g_ucs4_to_utf16 (str1, 12, &items_read, &items_written, &err);
	check_result = ucs4_to_utf16_check_result (res, exp1, items_read, 11, items_written, 11, err, FALSE);
	if (check_result) return check_result;
	g_free (res);

	items_read = items_written = 0;
	res = g_ucs4_to_utf16 (str2, 0, &items_read, &items_written, &err);
	check_result = ucs4_to_utf16_check_result (res, exp2, items_read, 0, items_written, 0, err, FALSE);
	if (check_result) return check_result;
	g_free (res);

	items_read = items_written = 0;
	res = g_ucs4_to_utf16 (str2, 1, &items_read, &items_written, &err);
	check_result = ucs4_to_utf16_check_result (res, exp2, items_read, 1, items_written, 1, err, FALSE);
	if (check_result) return check_result;
	g_free (res);

	items_read = items_written = 0;
	res = g_ucs4_to_utf16 (str2, 2, &items_read, &items_written, &err);
	check_result = ucs4_to_utf16_check_result (res, 0, items_read, 1, items_written, 0, err, TRUE);
	if (check_result) return check_result;
	g_free (res);

	items_read = items_written = 0;
	err = 0;
	res = g_ucs4_to_utf16 (str3, 2, &items_read, &items_written, &err);
	check_result = ucs4_to_utf16_check_result (res, 0, items_read, 1, items_written, 0, err, TRUE);
	if (check_result) return check_result;
	g_free (res);

	items_read = items_written = 0;
	err = 0;
	res = g_ucs4_to_utf16 (str4, 5, &items_read, &items_written, &err);
	check_result = ucs4_to_utf16_check_result (res, exp4, items_read, 2, items_written, 3, err, FALSE);
	if (check_result) return check_result;
	g_free (res);

	// This loop tests the bounds of the conversion algorithm
	current_write_index = 0;
	for (i=0;i<6;i++) {
		items_read = items_written = 0;
		err = 0;
		res = g_ucs4_to_utf16 (&str5[i], 1, &items_read, &items_written, &err);
		check_result = ucs4_to_utf16_check_result (res, &exp5[current_write_index], 
					items_read, read_write[i*2], items_written, read_write[(i*2)+1], err, !read_write[(i*2)+1]);
		if (check_result) return check_result;
		g_free (res);
		current_write_index += items_written;
	}

	return OK;
}

static RESULT
utf16_to_ucs4_check_result (const gunichar *result_str, const gunichar *expected_str,
			    glong result_items_read, glong expected_items_read,
			    glong result_items_written, glong expected_items_written,
			    GError* result_error, gboolean expect_error)
{
	glong i;
	if (result_items_read != expected_items_read)
		return FAILED("Incorrect number of items read %d", result_items_read);
	if (result_items_written != expected_items_written)
		return FAILED("Incorrect number of items written %d", result_items_written);
	if (result_error && !expect_error)
		return FAILED("There should not be an error code.");
	if (!result_error && expect_error)
		return FAILED("Unexpected error object.");
	if (expect_error && result_str)
		return FAILED("NULL should be returned when an error occurs.");
	if (!expect_error && !result_str)
		return FAILED("When no error occurs NULL should not be returned.");
	for (i=0; i<expected_items_written;i++) {
		if (result_str [i] != expected_str [i])
			return FAILED("Incorrect value %d at index %d", result_str [i], i);
	}
	if (result_str && result_str[expected_items_written] != '\0') 
		return FAILED("Null termination not found at the end of the string.");
	
	return OK;
}

RESULT
test_utf16_to_ucs4 ()
{
	static gunichar2 str1[12] = {'H','e','l','l','o',' ','W','o','r','l','d','\0'};
	static gunichar exp1[12] = {'H','e','l','l','o',' ','W','o','r','l','d','\0'};
	static gunichar2 str2[7] = {'H', 0xD800, 0xDC01,0xD800,0xDBFF,'l','\0'};
	static gunichar exp2[3] = {'H',0x00010001,'\0'};
	static gunichar2 str3[4] = {'H', 0xDC00 ,'l','\0'};
	static gunichar exp3[2] = {'H','\0'};
	static gunichar2 str4[20] = {0xDC00,0xDFFF,0xDFF,0xD800,0xDBFF,0xD800,0xDC00,0xD800,0xDFFF,
				     0xD800,0xE000,0xDBFF,0xDBFF,0xDBFF,0xDC00,0xDBFF,0xDFFF,0xDBFF,0xE000,'\0'};
	static gunichar exp4[6] = {0xDFF,0x10000,0x103ff,0x10fc00,0x10FFFF,'\0'};
	static glong read_write[33] = {1,0,0,1,0,0,1,1,1,2,1,0,2,2,1,2,2,1,2,1,0,2,1,0,2,2,1,2,2,1,2,1,0};
	gunichar* res;
	glong items_read, items_written, current_read_index,current_write_index;
	GError* err=0;
	RESULT check_result;
	glong i;
	
	res = g_utf16_to_ucs4 (str1, 12, &items_read, &items_written, &err);
	check_result = utf16_to_ucs4_check_result (res, exp1, items_read, 11, items_written, 11, err, FALSE);
	if (check_result) return check_result;
	g_free (res);

	items_read = items_written = 0;
	res = g_utf16_to_ucs4 (str2, 0, &items_read, &items_written, &err);
	check_result = utf16_to_ucs4_check_result (res, exp2, items_read, 0, items_written, 0, err, FALSE);
	if (check_result) return check_result;
	g_free (res);

	items_read = items_written = 0;
	res = g_utf16_to_ucs4 (str2, 1, &items_read, &items_written, &err);
	check_result = utf16_to_ucs4_check_result (res, exp2, items_read, 1, items_written, 1, err, FALSE);
	if (check_result) return check_result;
	g_free (res);
	
	items_read = items_written = 0;
	res = g_utf16_to_ucs4 (str2, 2, &items_read, &items_written, &err);
	check_result = utf16_to_ucs4_check_result (res, exp2, items_read, 1, items_written, 1, err, FALSE);
	if (check_result) return check_result;
	g_free (res);

	items_read = items_written = 0;
	res = g_utf16_to_ucs4 (str2, 3, &items_read, &items_written, &err);
	check_result = utf16_to_ucs4_check_result (res, exp2, items_read, 3, items_written, 2, err, FALSE);
	if (check_result) return check_result;
	g_free (res);

	items_read = items_written = 0;
	res = g_utf16_to_ucs4 (str2, 4, &items_read, &items_written, &err);
	check_result = utf16_to_ucs4_check_result (res, exp2, items_read, 3, items_written, 2, err, FALSE);
	if (check_result) return check_result;
	g_free (res);

	items_read = items_written = 0;
	res = g_utf16_to_ucs4 (str2, 5, &items_read, &items_written, &err);
	check_result = utf16_to_ucs4_check_result (res, exp2, items_read, 4, items_written, 0, err, TRUE);
	if (check_result) return check_result;
	g_free (res);

	items_read = items_written = 0;
	err = 0;
	res = g_utf16_to_ucs4 (str3, 5, &items_read, &items_written, &err);
	check_result = utf16_to_ucs4_check_result (res, exp3, items_read, 1, items_written, 0, err, TRUE);
	if (check_result) return check_result;
	g_free (res);

	// This loop tests the bounds of the conversion algorithm
	current_read_index = current_write_index = 0;
	for (i=0;i<11;i++) {
		items_read = items_written = 0;
		err = 0;
		res = g_utf16_to_ucs4 (&str4[current_read_index], read_write[i*3], &items_read, &items_written, &err);
		check_result = utf16_to_ucs4_check_result (res, &exp4[current_write_index], items_read, 
					     read_write[(i*3)+1], items_written, read_write[(i*3)+2], err, 
					     !read_write[(i*3)+2]);
		if (check_result) return check_result;
		g_free (res);
		current_read_index += read_write[i*3];
		current_write_index += items_written;
	}

	return OK;
}

/*
 * test initialization
 */

static Test utf8_tests [] = {
	{"g_utf16_to_utf8", test_utf16_to_utf8},
	{"g_utf8_to_utf16", test_utf8_to_utf16},
	{"g_utf8_seq", test_utf8_seq},
	{"g_convert", test_convert },
	{"g_unichar_xdigit_value", test_xdigit },
	{"g_ucs4_to_utf16", test_ucs4_to_utf16 },
	{"g_utf16_to_ucs4", test_utf16_to_ucs4 },
	{NULL, NULL}
};

DEFINE_TEST_GROUP_INIT(utf8_tests_init, utf8_tests)


