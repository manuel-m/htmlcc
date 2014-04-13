#ifndef QDIRTY_H
#define	QDIRTY_H

#ifdef	__cplusplus
extern "C" {
#endif

#define QD_DECL_STR(MYSTR) static const char MYSTR##_str[] = # MYSTR
#define QD_STR(MYSTR) MYSTR##_str

#ifdef	__cplusplus
}
#endif

#endif	/* QDIRTY_H */

