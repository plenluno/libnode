/*								      HTParse.c
**	URI MANAGEMENT
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id: HTParse.c,v 2.80 1999/02/22 22:10:11 frystyk Exp $
**
** history:
**	May 12 94	TAB added as legal char in HTCleanTelnetString
**
*/

/* Library include files */
#include "wwwsys.h"
#include "WWWUtil.h"
#include "HTParse.h"					 /* Implemented here */

typedef struct _HTURI {
    char * access;		/* Now known as "scheme" */
    char * host;
    char * absolute;
    char * relative;
    char * fragment;
} HTURI;

/*	Scan a filename for its consituents
**	-----------------------------------
**
** On entry,
**	name	points to a document name which may be incomplete.
** On exit,
**      absolute or relative may be nonzero (but not both).
**	host, fragment and access may be nonzero if they were specified.
**	Any which are nonzero point to zero terminated strings.
*/
PRIVATE void scan (char * name, HTURI * parts)
{
    char * p;
    char * after_access = name;
    memset(parts, '\0', sizeof(HTURI));

    /* Look for fragment identifier */
    if ((p = strchr(name, '#')) != NULL) {
	*p++ = '\0';
	parts->fragment = p;
    }
    
    
    if ((p = strchr(name, ' ')) != NULL) *p++ = '\0';    
    
    for(p=name; *p; p++) {

	/* 
	** Look for any whitespace. This is very bad for pipelining as it
	** makes the request invalid
	*/
	if (isspace((int) *p)) {
	    char *orig=p, *dest=p+1;
	    while ((*orig++ = *dest++));
	    p = p-1;
	}
	if (*p=='/' || *p=='#' || *p=='?')
	    break;
	if (*p==':') {
		*p = 0;
		parts->access = after_access; /* Scheme has been specified */

/* The combination of gcc, the "-O" flag and the HP platform is
   unhealthy. The following three lines is a quick & dirty fix, but is
   not recommended. Rather, turn off "-O". */

/*		after_access = p;*/
/*		while (*after_access == 0)*/
/*		    after_access++;*/

		after_access = p+1;

		if (0==strcasecomp("URL", parts->access)) {
		    parts->access = NULL;  /* Ignore IETF's URL: pre-prefix */
		} else break;
	}
    }
    
    p = after_access;
    if (*p=='/'){
	if (p[1]=='/') {
	    parts->host = p+2;		/* host has been specified 	*/
	    *p=0;			/* Terminate access 		*/
	    p=strchr(parts->host,'/');	/* look for end of host name if any */
	    if(p) {
	        *p=0;			/* Terminate host */
	        parts->absolute = p+1;		/* Root has been found */
	    }
	} else {
	    parts->absolute = p+1;		/* Root found but no host */
	}	    
    } else {
        parts->relative = (*after_access) ? after_access : 0; /* zero for "" */
    }
}


/*	Parse a Name relative to another name
**	-------------------------------------
**
**	This returns those parts of a name which are given (and requested)
**	substituting bits from the related name where necessary.
**
** On entry,
**	aName		A filename given
**      relatedName     A name relative to which aName is to be parsed. Give
**                      it an empty string if aName is absolute.
**      wanted          A mask for the bits which are wanted.
**
** On exit,
**	returns		A pointer to a malloc'd string which MUST BE FREED
*/
PUBLIC char * HTParse (const char *aName, const char *relatedName, int wanted)
{
    char * result = 0;
    char * return_value = 0;
    int len;
    char * name = 0;
    char * rel = 0;
    char * p;
    char * access;
    HTURI given, related;
    
    if (!aName) return NULL;
    if (!relatedName)        /* HWL 23/8/94: dont dump due to NULL */
        relatedName = "";
    
    /* Make working copies of input strings to cut up: */
    len = strlen(aName)+strlen(relatedName)+10;
    if ((result=(char *) HT_MALLOC(len)) == NULL) /* Lots of space: more than enough */
	HT_OUTOFMEM("parse space");
    StrAllocCopy(name, aName);
    StrAllocCopy(rel, relatedName);
    
    scan(name, &given);
    scan(rel,  &related); 
    result[0]=0;		/* Clear string  */
    access = given.access ? given.access : related.access;
    if (wanted & PARSE_ACCESS)
        if (access) {
	    strcat(result, access);
	    if(wanted & PARSE_PUNCTUATION) strcat(result, ":");
	}
	
    if (given.access && related.access)	/* If different, inherit nothing. */
        if (strcmp(given.access, related.access)!=0) {
	    related.host=0;
	    related.absolute=0;
	    related.relative=0;
	    related.fragment=0;
	}
	
    if (wanted & PARSE_HOST)
        if(given.host || related.host) {
	    if(wanted & PARSE_PUNCTUATION) strcat(result, "//");
	    strcat(result, given.host ? given.host : related.host);
	}
	
    if (given.host && related.host)  /* If different hosts, inherit no path. */
        if (strcmp(given.host, related.host)!=0) {
	    related.absolute=0;
	    related.relative=0;
	    related.fragment=0;
	}
	
    if (wanted & PARSE_PATH) {
        if(given.absolute) {				/* All is given */
	    if(wanted & PARSE_PUNCTUATION) strcat(result, "/");
	    strcat(result, given.absolute);
	} else if(related.absolute) {	/* Adopt path not name */
	    strcat(result, "/");
	    strcat(result, related.absolute);
	    if (given.relative) {
		p = strchr(result, '?');	/* Search part? */
		if (!p) p=result+strlen(result)-1;
		for (; *p!='/'; p--);	/* last / */
		p[1]=0;					/* Remove filename */
		strcat(result, given.relative);		/* Add given one */
#if 0
		result = HTSimplify (&result);
#endif
	    }
	} else if(given.relative) {
	    strcat(result, given.relative);		/* what we've got */
	} else if(related.relative) {
	    strcat(result, related.relative);
	} else {  /* No inheritance */
	    strcat(result, "/");
	}
    }
		
    if (wanted & PARSE_VIEW)
	if(given.fragment || related.fragment) {
	    if(given.absolute && given.fragment) {   /*Fixes for relURLs...*/
		if(wanted & PARSE_PUNCTUATION) strcat(result, "#");
		strcat(result, given.fragment); 
	    } else if (!(given.absolute) && !(given.fragment)) {
		strcat(result, "");
	    } else {
		if(wanted & PARSE_PUNCTUATION) strcat(result, "#");
		strcat(result, given.fragment ? given.fragment : related.fragment); 
	    }
	}
    HT_FREE(rel);
    HT_FREE(name);
    
    StrAllocCopy(return_value, result);
    HT_FREE(result);
    return return_value;		/* exactly the right length */
}


/*
**	Canonicalizes the URL in the following manner starting from the host
**	pointer:
**
**	1) The host name is converted to lowercase
**	2) Chop off port if `:80' (http), `:70' (gopher), or `:21' (ftp)
**
**	Return: OK	The position of the current path part of the URL
**			which might be the old one or a new one.
*/
PRIVATE char * HTCanon (char ** filename, char * host)
{
    char *newname = NULL;
    char *port;
    char *strptr;
    char *path;
    char *access = host-3;

    while (access>*filename && *(access-1)!='/')       /* Find access method */
	access--;
    if ((path = strchr(host, '/')) == NULL)			/* Find path */
	path = host + strlen(host);
    if ((strptr = strchr(host, '@')) != NULL && strptr<path)	   /* UserId */
	host = strptr;
    if ((port = strchr(host, ':')) != NULL && port>path)      /* Port number */
	port = NULL;

    strptr = host;				    /* Convert to lower-case */
    while (strptr<path) {
	*strptr = TOLOWER(*strptr);
	strptr++;
    }
    
    /* Does the URL contain a full domain name? This also works for a
       numerical host name. The domain name is already made lower-case
       and without a trailing dot. */
#if 0
    if (((strptr = strchr(host, '.')) == NULL || strptr >= path) &&
	strncasecomp(host, "localhost", 9)) {
	const char *domain = HTGetDomainName();
	if (domain && *domain) {
	    if ((newname = (char *) HT_CALLOC(1, strlen(*filename) + strlen(domain)+2)) == NULL)
		HT_OUTOFMEM("HTCanon");
	    if (port)
		strncpy(newname, *filename, (int) (port-*filename));
	    else
		strncpy(newname, *filename, (int) (path-*filename));
	    strcat(newname, ".");
	    strcat(newname, domain);
	}
    } else 					  /* Look for a trailing dot */
#endif
    {
	char *dot = port ? port : path;
	if (dot > *filename && *--dot=='.') {
	    char *orig=dot, *dest=dot+1;
	    while((*orig++ = *dest++));
	    if (port) port--;
	    path--;
	}
    }
    /* Chop off port if `:', `:80' (http), `:70' (gopher), or `:21' (ftp) */
    if (port) {
	if (!*(port+1) || *(port+1)=='/') {
	    if (!newname) {
		char *orig=port, *dest=port+1;
		while((*orig++ = *dest++));
	    }
	} else if ((!strncmp(access, "http", 4) &&
	     (*(port+1)=='8'&&*(port+2)=='0'&&(*(port+3)=='/'||!*(port+3)))) ||
	    (!strncmp(access, "gopher", 6) &&
	     (*(port+1)=='7'&&*(port+2)=='0'&&(*(port+3)=='/'||!*(port+3)))) ||
	    (!strncmp(access, "ftp", 3) &&
	     (*(port+1)=='2'&&*(port+2)=='1'&&(*(port+3)=='/'||!*(port+3))))) {
	    if (!newname) {
		char *orig=port, *dest=port+3;
		while((*orig++ = *dest++));
		path -= 3;   	       /* Update path position, Henry Minsky */
	    }
	} else if (newname)
	    strncat(newname, port, (int) (path-port));
    }

    if (newname) {
	char *newpath = newname+strlen(newname);
	strcat(newname, path);
	path = newpath;
	HT_FREE(*filename);				    /* Free old copy */
	*filename = newname;
    }
    return path;
}

/*
**  Search the URL and determine whether it is a relative or absolute URL.
**  We check to see if there is a ":" before any "/", "?", and "#". If this
**  is the case then we say it is absolute. Otherwise it is relative.
*/
PUBLIC BOOL HTURL_isAbsolute (const char * url)
{    
    if (url) {	
	const char * ptr = url;
	while (*ptr) {
	    if (*ptr == ':') return YES;
	    if (*ptr == '/' || *ptr == '?' || *ptr == '#') break;
	    ptr ++;
	}
    }	
    return NO;
}

/*	        Simplify a URI
//		--------------
// A URI is allowed to contain the seqeunce xxx/../ which may be
// replaced by "" , and the seqeunce "/./" which may be replaced by "/".
// Simplification helps us recognize duplicate URIs. 
//
//	Thus, 	/etc/junk/../fred 	becomes	/etc/fred
//		/etc/junk/./fred	becomes	/etc/junk/fred
//
//      but we should NOT change
//		http://fred.xxx.edu/../..
//
//	or	../../albert.html
//
// In order to avoid empty URLs the following URLs become:
//
//		/fred/..		becomes /fred/..
//		/fred/././..		becomes /fred/..
//		/fred/.././junk/.././	becomes /fred/..
//
// If more than one set of `://' is found (several proxies in cascade) then
// only the part after the last `://' is simplified.
//
// Returns: A string which might be the old one or a new one.
*/
PUBLIC char *HTSimplify (char ** url)
{
    char *path;
    char *p;
    if (!url || !*url) {
	HTTRACE(URI_TRACE, "HTSimplify.. Nothing done\n");
	return *url;
    }
    HTTRACE(URI_TRACE, "HTSimplify.. `%s\' " _ *url);

    /* Find any scheme name */
    if ((path = strstr(*url, "://")) != NULL) {		   /* Find host name */
	char *newptr;
	char *access = *url;
	while (access<path && (*access=TOLOWER(*access))) access++;
	path += 3;
	while ((newptr = strstr(path, "://")) != NULL)        /* For proxies */
	    path = newptr+3;
	path = HTCanon(url, path);       	      /* We have a host name */
    } else if ((path = strstr(*url, ":/")) != NULL) {
	path += 2;
    } else
	path = *url;
    if (*path == '/' && *(path+1)=='/') {	  /* Some URLs start //<foo> */
	path += 1;
    } else if (!strncmp(path, "news:", 5)) {
	char *ptr = strchr(path+5, '@');
	if (!ptr) ptr = path+5;
	while (*ptr) {			    /* Make group or host lower case */
	    *ptr = TOLOWER(*ptr);
	    ptr++;
	}
	HTTRACE(URI_TRACE, "into\n............ `%s'\n" _ *url);
	return *url;		      /* Doesn't need to do any more */
    }
    if ((p = path)) {
	char *end;
	if (!((end = strchr(path, ';')) || (end = strchr(path, '?')) ||
	      (end = strchr(path, '#'))))
	    end = path+strlen(path);

	/* Parse string second time to simplify */
	p = path;
	while(p<end) {
	    if (*p=='/') {
		if (p>*url && *(p+1)=='.' && (*(p+2)=='/' || !*(p+2))) {
		    char *orig = p+1;
		    char *dest = (*(p+2)!='/') ? p+2 : p+3;
		    while ((*orig++ = *dest++)); /* Remove a slash and a dot */
		    end = orig-1;
		} else if (*(p+1)=='.' && *(p+2)=='.' && (*(p+3)=='/' || !*(p+3))) {
		    char *q = p;
		    while (q>path && *--q!='/');	       /* prev slash */
		    if (strncmp(q, "/../", 4)) {
			char *orig = q+1;
			char *dest = (*(p+3)!='/') ? p+3 : p+4;
			while ((*orig++ = *dest++));	   /* Remove /xxx/.. */
			end = orig-1;
			p = q;		      /* Start again with prev slash */
		    } else
			p++;
		} else if (*(p+1)=='/') {
		    while (*(p+1)=='/') {
			char *orig=p, *dest=p+1;
			while ((*orig++ = *dest++));  /* Remove multiple /'s */
			end = orig-1;
		    }
		} else
		    p++;
	    } else
		p++;
	}
    }

    /*
    **  Check for host/../.. kind of things
    */
    while (*path=='/' && *(path+1)=='.' && *(path+2)=='.' &&
	   (!*(path+3) || *(path+3)=='/')) {
	char * orig = path;
	char * dest = path+3;
	while ((*orig++ = *dest++));
    }
    HTTRACE(URI_TRACE, "into\n............ `%s'\n" _ *url);
    return *url;
}

/*		Make Relative Name
**		------------------
**
** This function creates and returns a string which gives an expression of
** one address as related to another. Where there is no relation, an absolute
** address is retured.
**
**  On entry,
**	Both names must be absolute, fully qualified names of nodes
**	(no fragment bits)
**
**  On exit,
**	The return result points to a newly allocated name which, if
**	parsed by HTParse relative to relatedName, will yield aName.
**	The caller is responsible for freeing the resulting name later.
**
*/
PUBLIC char * HTRelative (const char * aName, const char * relatedName)
{
    char * result = 0;
    const char *p = aName;
    const char *q = relatedName;
    const char * after_access = NULL;
    const char * path = 0;
    const char * last_slash = 0;
    int slashes = 0;
    
    for(;*p; p++, q++) {	/* Find extent of match */
    	if (*p!=*q) break;
	if (*p==':') if (!after_access) after_access = p+1;
	if (*p=='/') {
	    last_slash = p;
	    slashes++;
	    if (slashes==3) path=p;
	}
    }
    
    /* q, p point to the first non-matching character or zero */
    
    if (!after_access) {			/* Different access */
        StrAllocCopy(result, aName);
    } else if (slashes<3){			/* Different nodes */
    	StrAllocCopy(result, after_access);
    } else {					/* Some path in common */
        int levels= 0;
        for(; *q && *q!='#' && *q!=';' && *q!='?'; q++) if (*q=='/') levels++;
	if ((result = (char  *) HT_MALLOC(3*levels + strlen(last_slash) + 4)) == NULL)
	    HT_OUTOFMEM("HTRelative");
	*result = '\0';
	if (!levels) strcat(result, "./");
	for(;levels; levels--)strcat(result, "../");
	strcat(result, last_slash+1);
	if (!*result) strcat(result, "./");
    }
    HTTRACE(URI_TRACE, "HTRelative.. `%s' expressed relative to  `%s' is `%s'\n" _ 
		aName _ relatedName _ result);
#if 0
    {
	char * absstr = HTParse(result, relatedName, PARSE_ALL);
	HTSimplify(&absstr);
	HTTRACE(URI_TRACE, "HTRelative.. `%s' made absolute based on `%s' is `%s'\n" _
		result _ relatedName _ absstr);
	if (strcmp(absstr, aName) != 0) HTTRACE(URI_TRACE, "THEY DIFFER!!!\n");
	HT_FREE(absstr);
    }
#endif
    return result;
}

/*							HTCleanTelnetString()
 *	Make sure that the given string doesn't contain characters that
 *	could cause security holes, such as newlines in ftp, gopher,
 *	news or telnet URLs; more specifically: allows everything between
 *	ASCII 20-7E, and also A0-FE, inclusive. Also TAB ('\t') allowed!
 *
 * On entry,
 *	str	the string that is *modified* if necessary.  The
 *		string will be truncated at the first illegal
 *		character that is encountered.
 * On exit,
 *	returns	YES, if the string was modified.
 *		NO, otherwise.
 */
PUBLIC BOOL HTCleanTelnetString (char * str)
{
    char * cur = str;

    if (!str) return NO;

    while (*cur) {
	int a = TOASCII((unsigned char) *cur);
	if (a != 0x9 && (a < 0x20 || (a > 0x7E && a < 0xA0) ||  a > 0xFE)) {
	    HTTRACE(URI_TRACE, "Illegal..... character in URL: \"%s\"\n" _ str);
	    *cur = 0;
	    HTTRACE(URI_TRACE, "Truncated... \"%s\"\n" _ str);
	    return YES;
	}
	cur++;
    }
    return NO;
}

