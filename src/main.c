#include <stdio.h>
#include <time.h>
#include <string.h>

// Defines memory allocations

#define STRMEM 4096 * 96
#define CONMEM 1000
#define ITEMS 128

#define NAME "Thomasorus"
#define DOMAIN "https://thomasorus.com/"
#define LOCATION "France"
#define REPOPATH ""

typedef struct Block {
	int len;
	char data[STRMEM];
} Block;

// typedef struct List {
// 	int len, routes;
// 	char *name, *keys[ITEMS], *vals[ITEMS];
// } List;

// Term is an entry of the content
typedef struct Term {
	int body_len;
	int children_len;
	int incoming_len;
	int outgoing_len;
	int logs_len;
	int events_len;
	int ch;
	int fh;
	char *name;
	char *host;
	char *bref;
	char *priv;
	char *type;
	char *filename;
	char *date_from;
	char *date_last;
	char *body[ITEMS];
	// struct List link;
	struct Term *parent;
	struct Term *children[ITEMS];
	struct Term *incoming[ITEMS];
} Term;

// Content takes multiple terms
typedef struct Content {
	int len;
	Term terms[CONMEM];
} Content;

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

// typedef void(*split_fn)(const char *, size_t, void *);
// void split(const char *str, char sep, split_fn fun, void *data)
// {
//     unsigned int start = 0, stop;
//     for (stop = 0; str[stop]; stop++) {
//         if (str[stop] == sep) {
//             fun(str + start, stop - start, data);
//             start = stop + 1;
//         }
//     }
//     fun(str + start, stop - start, data);
// }

// void print(const char *str, size_t len, void *data)
// {
//     printf("%.*s\n", (int)len, str);
// }

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

#pragma mark - Term

Term *
maketerm(Term *t, char *name)
{
	t->body_len = 0;
	t->children_len = 0;
	t->incoming_len = 0;
	t->outgoing_len = 0;
	t->logs_len = 0;
	t->events_len = 0;
	t->ch = 0;
	t->fh = 0;
	t->priv = 0;
	t->name = stlc(name);
	return t;
}

#pragma mark - Block

char *
push(Block *b, char *s)
{
	int i = 0, o = b->len;
	while(s[i]) {
		b->data[b->len++] = s[i++];
	}
	b->data[b->len++] = '\0';
	return &b->data[o];
}

#pragma mark - Time

float
clockoffset(clock_t start)
{
	return (((double)(clock() - start)) / CLOCKS_PER_SEC) * 1000;
}

#pragma mark - File

FILE *
getfile(char *dir, char *filename, char *ext, char *op)
{
	char filepath[1024];
	filepath[0] = '\0';
	scat(filepath, dir);
	scat(filepath, filename);
	scat(filepath, ext);
	scat(filepath, "\0");
	return fopen(filepath, op);
}

#pragma mark - Parse

int
parse_content(FILE *fp, Block *block, Content *con)
{
	int key_len, val_len, len, count = 0, catch_body = 0, catch_link = 0;
	char line[1024], buf[1024];
	Term *t = &con->terms[con->len];
	while(fgets(line, 1024, fp)) {
		strm(line);
		// printf("%s\n", line);
		len = slen(line);
		count++;
		if(len < 3 || line[0] == ';')
			continue;
		if(con->len >= CONMEM)
			return errorid("Increase memory", "Content", con->len);
		if(len > 1024)
			return errorid("Line is too long", line, len);
		if(ssin(line, "====") >= 0) {
			printf("%s\n", line);
		}
		if(ssin(line, "NAME: ") >= 0) {
			t = maketerm(&con->terms[con->len++], push(block, sstr(line, buf, 6, len - 6)));
			t->name = push(block, sstr(line, buf, 6, len - 6));
			t->filename = push(block, scsw(stlc(sstr(t->name, buf, 0, len)), ' ', '-'));
		}
		if(ssin(line, "HOST: ") >= 0) {
			t->host = push(block, sstr(line, buf, 6, len - 6));
		}
		if(ssin(line, "BREF: ") >= 0) {
			t->bref = push(block, sstr(line, buf, 6, len - 6));
		}
		if(ssin(line, "PRIV: ") >= 0) {
			t->priv = push(block, sstr(line, buf, 6, len - 6));
		}
		if(ssin(line, "BODY:") >= 0) {
			catch_body = ssin(line, "BODY:") >= 0;
		}
		if(catch_body) {
			t->body[t->body_len++] = push(block, line);
		}
		// printf("%s\n", t->body_len);
		// else {
		// 	return errorid("Invalid line", line, count);
		// }
	}
	printf(":%d ", count);
	return 1;
}

parse(Block *block, Content *con)
{
	// FILE *fcon = fopen("database/lexicon.ndtl", "r");
	FILE *fcon = fopen("database/content.kaku", "r");

	printf("Parsing  | ");
	printf("Content");
	if(!fcon || !parse_content(fcon, block, con)) {
		fclose(fcon);
		return error("Parsing", "content");
	}
	fclose(fcon);
	return 1;
}

void
fpbodypart(FILE *f, Content *con, Term *t)
{
	int i;
	for(i = 0; i < t->body_len; ++i) {
		printf("%s\n", t->name);
		fprintf(f, "%s\n", t->body[i]);
	}
	// Parser goes here
	// fptemplate(f, con, t, t->body[i]);
	// fprintf(f, "%s\n", t->body[i]);
}

void
fpbody(FILE *f, Content *con, Term *t)
{
	fprintf(f, "%s\n", t->bref);
	fpbodypart(f, con, t);
}

void
fphtml(FILE *f, Content *con, Term *t)
{

	// printf("%s\n", DOMAIN);
	// Term *alias = NULL;
	// if(t->type && scmp(t->type, "alias"))
	// 	alias = findterm(con, t->host);
	fputs("<!DOCTYPE html><html lang='en'>", f);
	fputs("<head>", f);
	fprintf(f, "<meta charset='utf-8'>"
			   "<meta name='description' content='%s'/>"
			   "<meta name='thumbnail' content='" DOMAIN "media/services/thumbnail.jpg' />"
			   "<meta name='viewport' content='width=device-width,initial-scale=1'>"
			   "<link rel='alternate' type='application/rss+xml' title='RSS Feed' href='../links/rss.xml' />"
			   "<link rel='stylesheet' type='text/css' href='../links/main.css'>"
			   "<link rel='shortcut icon' type='image/png' href='../media/services/icon.png'>"
			   "<title>" NAME " &mdash; %s</title>",
		t->bref,
		t->name);
	fputs("</head>", f);
	fputs("<body>", f);
	// fputs("<header><a href='home.html'><img src='../media/icon/logo.svg' alt='" NAME "' height='29'></a></header>", f);
	// fpnav(f, alias ? alias : t);
	// fputs("<main>", f);
	// fpbanner(f, jou, alias ? alias : t, 1);
	fpbody(f, con, t);
	// fpinclude(f, alias ? alias->filename : t->filename, 0, 0);
	// if(t->type) {
	// 	if(scmp(t->type, "pict_portal"))
	// 		fpportal(f, glo, lex, jou, alias ? alias : t, 1, 0);
	// 	else if(scmp(t->type, "text_portal"))
	// 		fpportal(f, glo, lex, jou, alias ? alias : t, 0, 1);
	// 	else if(scmp(t->type, "album"))
	// 		fpalbum(f, jou, alias ? alias : t);
	// 	else if(scmp(t->type, "alias"))
	// 		fprintf(f, "<p>Redirected to <a href='%s.html'>%s</a>, from <b>%s</b>.</p>", alias->filename, alias->name, t->name);
	// 	else if(!scmp(t->type, "hidden"))
	// 		error("Unknown template", t->type);
	// }
	// if(scmp(t->name, "now"))
	// 	fpnow(f, lex, jou);
	// else if(scmp(t->name, "home"))
	// 	fphome(f, jou);
	// else if(scmp(t->name, "calendar"))
	// 	fpcalendar(f, jou);
	// else if(scmp(t->name, "tracker"))
	// 	fptracker(f, jou);
	// else if(scmp(t->name, "journal"))
	// 	fpjournal(f, jou);
	// else if(scmp(t->name, "index"))
	// 	fpindex(f, lex, jou);
	// fplinks(f, alias ? alias : t);
	// fpincoming(f, alias ? alias : t);
	// fphoraire(f, jou, alias ? alias : t);
	// fputs("</main>", f);
	// fputs("<footer>", f);
	// fputs("<a href='https://creativecommons.org/licenses/by-nc-sa/4.0'><img src='../media/icon/cc.svg' width='30'/></a>", f);
	// fputs("<a href='http://webring.xxiivv.com/'><img src='../media/icon/rotonde.svg' width='30'/></a>", f);
	// fputs("<a href='https://merveilles.town/@neauoire'><img src='../media/icon/merveilles.svg' width='30'/></a>", f);
	// fputs("<a href='https://github.com/neauoire'><img src='../media/icon/github.png' alt='github' width='30'/></a>", f);
	// fputs("<span><a href='devine_lu_linvega.html'>Devine Lu Linvega</a> &copy; 2021 &mdash; <a href='about.html'>BY-NC-SA 4.0</a></span>", f);
	// fputs("</footer>", f);
	fputs("</body></html>", f);
	fclose(f);
}

int
build(Content *con)
{
	FILE *f;
	int i;

	printf("Building | ");
	printf("%d pages ", con->len);
	for(i = 0; i < con->len; ++i) {
		f = getfile("../site/", con->terms[i].filename, ".html", "w");
		if(!f)
			return error("Could not open file", con->terms[i].name);
		fphtml(f, con, &con->terms[i]);
	}
	// printf("2 feeds ");
	// f = fopen("../links/rss.xml", "w");
	// if(!f)
	// 	return error("Could not open file", "rss.xml");
	// fprss(f, jou);
	// f = fopen("../links/tw.txt", "w");
	// if(!f)
	// 	return error("Could not open file", "tw.txt");
	// fptwtxt(f, jou);
	return 1;
}

Block block;
Content all_content;
clock_t start;

int
main(void)
{
	puts("");

	start = clock();
	if(!parse(&block, &all_content))
		return error("Failure", "Parsing");
	printf("[%.2fms]\n", clockoffset(start));

	start = clock();
	if(!build(&all_content))
		return error("Failure", "Building");
	printf("[%.2fms]\n", clockoffset(start));

	printf("%d/%d characters in memory\n", block.len, STRMEM);

	return 0;
}
