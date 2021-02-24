#include <stdio.h>
#include <time.h>

#define STRMEM 4096 * 96
#define CONMEM 1000
#define ITEMS 128

typedef struct Block {
	int len;
	char data[STRMEM];
} Block;

typedef struct TimeLog {
	int *year, *week, *hours;
	char *activity, *project;
} TimeLog;

// Content takes multiple terms
typedef struct Week {
	int len;
	TimeLog timelog[CONMEM];
} Week;

typedef struct Year {
	int len;
	Week week[CONMEM];
} Year;

#pragma mark - Helpers

/* clang-format off */

int   cisp(char c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r'; } /* char is space */
int   cial(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); } /* char is alpha */
int   cinu(char c) { return c >= '0' && c <= '9'; } /* char is num */
char  clca(char c) { return c >= 'A' && c <= 'Z' ? c + ('a' - 'A') : c; } /* char to lowercase */
char  cuca(char c) { return c >= 'a' && c <= 'z' ? c - ('a' - 'A') : c; } /* char to uppercase */
int   spad(char *s, char c) { int i = 0; while(s[i] && s[i] == c && s[++i]) { ; } return i; } /* string count padding */
int   slen(char *s) { int i = 0; while(s[i] && s[++i]) { ; } return i; } /* string length */
char *st__(char *s, char (*fn)(char)) { int i = 0; char c; while((c = s[i])) s[i++] = fn(c); return s; }
char *stuc(char *s) { return st__(s, cuca); } /* string to uppercase */
char *stlc(char *s) { return st__(s, clca); } /* string to lowercase */
char *scpy(char *src, char *dst, int len) { int i = 0; while((dst[i] = src[i]) && i < len - 2) i++; dst[i + 1] = '\0'; return dst; } /* string copy */
int   scmp(char *a, char *b) { int i = 0; while(a[i] == b[i]) if(!a[i++]) return 1; return 0; } /* string compare */
int   sint(char *s, int len) { int n = 0, i = 0; while(s[i] && i < len && (s[i] >= '0' && s[i] <= '9')) n = n * 10 + (s[i++] - '0'); return n; } /* string to num */
char *scsw(char *s, char a, char b) { int i = 0; char c; while((c = s[i])) s[i++] = c == a ? b : c; return s; } /* string char swap */
int   sian(char *s) { int i = 0; char c; while((c = s[i++])) if(!cial(c) && !cinu(c) && !cisp(c)) return 0; return 1; } /* string is alphanum */
int   scin(char *s, char c) { int i = 0; while(s[i]) if(s[i++] == c) return i - 1; return -1; } /* string char index */
char *scat(char *dst, const char *src) { char *ptr = dst + slen(dst); while(*src) *ptr++ = *src++; *ptr = '\0'; return dst; } /* string cat */
int   ssin(char *s, char *ss) { int a = 0, b = 0; while(s[a]) { if(s[a] == ss[b]) { if(!ss[b + 1]) return a - b; b++; } else b = 0; a++; } return -1; } /* string substring index */
char *strm(char *s) { char *end; while(cisp(*s)) s++; if(*s == 0) return s; end = s + slen(s) - 1; while(end > s && cisp(*end)) end--; end[1] = '\0'; return s; } /* string trim */
int   surl(char *s) { return ssin(s, "://") >= 0 || ssin(s, "../") >= 0; } /* string is url */
char *sstr(char *src, char *dst, int from, int to) { int i; char *a = (char *)src + from, *b = (char *)dst; for(i = 0; i < to; i++) b[i] = a[i]; dst[to] = '\0'; return dst; } /* String substring */
int   afnd(char *src[], int len, char *val) { int i; for(i = 0; i < len; i++) if(scmp(src[i], val)) return i; return -1; } /* Array find string */
char *ccat(char *dst, char c) { int len = slen(dst); dst[len] = c; dst[len + 1] = '\0'; return dst; } /* concatenate ? */

/* clang-format on */

#pragma mark - Core

int
error(char *msg, char *val)
{
	printf("Error: %s(%s)\n", msg, val);
	return 0;
}

int
errorid(char *msg, char *val, int id)
{
	printf("Error: %s:%d(%s)\n", msg, id, val);
	return 0;
}

#pragma mark - Time

float
clockoffset(clock_t start)
{
	return (((double)(clock() - start)) / CLOCKS_PER_SEC) * 1000;
}

#pragma mark - Term

TimeLog *
makeTimeLog(TimeLog *t, int *year)
{
	t->year = year;
	t->year = 0;
	t->hours = 0;
	t->activity = 0;
	t->project = 0;
	return t;
}

int
parse_content(FILE *fp, Block *block, Week *wee)
{
	int key_len, val_len, len, count = 0, catch_body = 0, catch_link = 0, skip_body = 0, init_size = 2000;
	char line[1024], buf[1024];
	TimeLog *w = &wee->timelog[wee->len];
	printf("\n");
	while(fgets(line, 1024, fp)) {
		strm(line);
		len = slen(line);
		count++;
		if(wee->len >= CONMEM)
			return errorid("Increase memory", "Content", wee->len);
		if(len > 1024)
			return errorid("Line is too long", line, len);

		char *delim = " ";
		char *ptr;
		ptr = strtok(line, delim);
		printf("%s\n", line);
		int i;
		for(i = 0; ptr != NULL; i++) {
			// printf("Adding line: %s \n", ptr);
			if(i == 0) {
				int year = sint(ptr, 4);
				w = makeTimeLog(&wee->timelog[wee->len++], year);
				// printf("%d\n", w->year);
				// printf("%ls\n", ptr);

			} else {
				ptr = strtok(NULL, delim);
				if(i == 1) {
					// w->week = push(block, ptr);
					// printf("%d\n", w->week);
					// printf("%d\n", ptr);
				}
			}
		}
	}
	printf(":%d ", count);
	return 1;
}

parse(Block *block, Week *wee)
{
	FILE *fcon = fopen("database/time.kaku", "r");

	printf("Parsing  | ");
	printf("Time");
	if(!fcon || !parse_content(fcon, block, wee)) {
		fclose(fcon);
		return error("Parsing", "time");
	}
	fclose(fcon);
	return 1;
}

Block block;
Week all_weeks;
clock_t start;

int
main(void)
{
	puts("");

	start = clock();
	if(!parse(&block, &all_weeks))
		return error("Failure", "Parsing");
	printf("[%.2fms]\n", clockoffset(start));

	return 0;
}