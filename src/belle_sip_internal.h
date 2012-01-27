/*
	belle-sip - SIP (RFC3261) library.
    Copyright (C) 2010  Belledonne Communications SARL

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
#ifndef belle_utils_h
#define belle_utils_h

#include <stdlib.h>
#include <string.h>

#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <unistd.h>

#ifndef WIN32
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif

/* include all public headers*/
#include "belle-sip/belle-sip.h"

typedef void (*belle_sip_object_destroy_t)(belle_sip_object_t*);
typedef void (*belle_sip_object_clone_t)(belle_sip_object_t* obj, const belle_sip_object_t *orig);
typedef int (*belle_sip_object_marshal_t)(belle_sip_object_t* obj, char* buff,unsigned int offset,size_t buff_size);

struct _belle_sip_object_vptr{
	belle_sip_type_id_t id;
	const char *type_name;
	int initially_unowned;
	struct _belle_sip_object_vptr *parent;
	belle_sip_interface_desc_t **interfaces; /*NULL terminated table of */
	belle_sip_object_destroy_t destroy;
	belle_sip_object_clone_t clone;
	belle_sip_object_marshal_t marshal;
};

typedef struct _belle_sip_object_vptr belle_sip_object_vptr_t;

extern belle_sip_object_vptr_t belle_sip_object_t_vptr;

#define BELLE_SIP_OBJECT_VPTR_NAME(object_type)	object_type##_vptr

#define BELLE_SIP_OBJECT_VPTR_TYPE(object_type)	object_type##_vptr_t

#define BELLE_SIP_DECLARE_VPTR(object_type) \
	typedef belle_sip_object_vptr_t BELLE_SIP_OBJECT_VPTR_TYPE(object_type);\
	extern BELLE_SIP_OBJECT_VPTR_TYPE(object_type) BELLE_SIP_OBJECT_VPTR_NAME(object_type);

#define BELLE_SIP_DECLARE_CUSTOM_VPTR_BEGIN(object_type, parent_type) \
	typedef struct object_type##_vptr_struct BELLE_SIP_OBJECT_VPTR_TYPE(object_type);\
	extern BELLE_SIP_OBJECT_VPTR_TYPE(object_type) BELLE_SIP_OBJECT_VPTR_NAME(object_type);\
	struct object_type##_vptr_struct{\
		BELLE_SIP_OBJECT_VPTR_TYPE(parent_type) base;

#define BELLE_SIP_DECLARE_CUSTOM_VPTR_END };

#define BELLE_SIP_INSTANCIATE_CUSTOM_VPTR(object_type) \
	BELLE_SIP_OBJECT_VPTR_TYPE(object_type) BELLE_SIP_OBJECT_VPTR_NAME(object_type)

#define BELLE_SIP_VPTR_INIT(object_type,parent_type,unowned) \
		BELLE_SIP_TYPE_ID(object_type), \
		#object_type,\
		unowned,\
		(belle_sip_object_vptr_t*)&BELLE_SIP_OBJECT_VPTR_NAME(parent_type), \
		(belle_sip_interface_desc_t**)object_type##interfaces_table


#define BELLE_SIP_INSTANCIATE_VPTR(object_type,parent_type,destroy,clone,marshal,unowned) \
		BELLE_SIP_OBJECT_VPTR_TYPE(object_type) BELLE_SIP_OBJECT_VPTR_NAME(object_type)={ \
		BELLE_SIP_VPTR_INIT(object_type,parent_type,unowned), \
		(belle_sip_object_destroy_t)destroy,	\
		(belle_sip_object_clone_t)clone,	\
		(belle_sip_object_marshal_t)marshal\
		}


#define BELLE_SIP_IMPLEMENT_INTERFACE_BEGIN(object_type,interface_name) \
	static BELLE_SIP_INTERFACE_METHODS_TYPE(interface_name)  methods_##object_type##_##interface_name={\
		{ BELLE_SIP_INTERFACE_ID(interface_name),\
		#interface_name },

#define BELLE_SIP_IMPLEMENT_INTERFACE_END };

#define BELLE_SIP_DECLARE_NO_IMPLEMENTED_INTERFACES(object_type)\
	static belle_sip_interface_desc_t * object_type##interfaces_table[]={\
		NULL \
	}

#define BELLE_SIP_DECLARE_IMPLEMENTED_INTERFACES_1(object_type,iface1) \
	static belle_sip_interface_desc_t * object_type##interfaces_table[]={\
		(belle_sip_interface_desc_t*)&methods_##object_type##_##iface1, \
		NULL \
	}

#define BELLE_SIP_DECLARE_IMPLEMENTED_INTERFACES_2(object_type,iface1,iface2) \
	static belle_sip_interface_desc_t * object_type##interfaces_table[]={\
		(belle_sip_interface_desc_t*)&methods_##object_type##_##iface1, \
		(belle_sip_interface_desc_t*)&methods_##object_type##_##iface2, \
		NULL \
	}

/*etc*/

#define BELLE_SIP_INTERFACE_GET_METHODS(obj,interface) \
	((BELLE_SIP_INTERFACE_METHODS_TYPE(interface)*)belle_sip_object_get_interface_methods((belle_sip_object_t*)obj,BELLE_SIP_INTERFACE_ID(interface)))

#define __BELLE_SIP_INVOKE_LISTENER_BEGIN(list,interface_name) \
	if (list!=NULL) {\
		const belle_sip_list_t *__elem=list;\
		do{\
			interface_name *__obj=(interface_name*)__elem->data;\
			BELLE_SIP_INTERFACE_GET_METHODS(__obj,interface_name)->

#define __BELLE_SIP_INVOKE_LISTENER_END \
			__elem=__elem->next;\
		}while(__elem!=NULL);\
	}

#define BELLE_SIP_INVOKE_LISTENERS_VOID(list,interface_name,method) \
			__BELLE_SIP_INVOKE_LISTENER_BEGIN(list,interface_name)\
			method(__obj);\
			__BELLE_SIP_INVOKE_LISTENER_END

#define BELLE_SIP_INVOKE_LISTENERS_ARG(list,interface_name,method,arg) \
	__BELLE_SIP_INVOKE_LISTENER_BEGIN(list,interface_name)\
	method(__obj,arg);\
	__BELLE_SIP_INVOKE_LISTENER_END


#define BELLE_SIP_INVOKE_LISTENERS_ARG1_ARG2(list,interface_name,method,arg1,arg2) \
			__BELLE_SIP_INVOKE_LISTENER_BEGIN(list,interface_name)\
			method(__obj,arg1,arg2);\
			__BELLE_SIP_INVOKE_LISTENER_END

#define BELLE_SIP_INVOKE_LISTENERS_ARG1_ARG2_ARG3(list,interface_name,method,arg1,arg2,arg3) \
			__BELLE_SIP_INVOKE_LISTENER_BEGIN(list,interface_name)\
			method(__obj,arg1,arg2,arg3);\
			__BELLE_SIP_INVOKE_LISTENER_END

typedef struct weak_ref{
	struct weak_ref *next;
	belle_sip_object_destroy_notify_t notify;
	void *userpointer;
}weak_ref_t;

struct _belle_sip_object{
	belle_sip_object_vptr_t *vptr;
	size_t size;
	int ref;
	char* name;
	weak_ref_t *weak_refs;
};

belle_sip_object_t * _belle_sip_object_new(size_t objsize, belle_sip_object_vptr_t *vptr);
void *belle_sip_object_get_interface_methods(belle_sip_object_t *obj, belle_sip_interface_id_t ifid);

#define belle_sip_object_new(_type) (_type*)_belle_sip_object_new(sizeof(_type),(belle_sip_object_vptr_t*)&BELLE_SIP_OBJECT_VPTR_NAME(_type))

#define BELLE_SIP_OBJECT_VPTR(obj,object_type) ((BELLE_SIP_OBJECT_VPTR_TYPE(object_type)*)(((belle_sip_object_t*)obj)->vptr))
#define belle_sip_object_init(obj)		/*nothing*/


/*list of all vptrs (classes) used in belle-sip*/
BELLE_SIP_DECLARE_VPTR(belle_sip_object_t);
BELLE_SIP_DECLARE_VPTR(belle_sip_stack_t);
BELLE_SIP_DECLARE_VPTR(belle_sip_datagram_listening_point_t);
BELLE_SIP_DECLARE_VPTR(belle_sip_provider_t);
BELLE_SIP_DECLARE_VPTR(belle_sip_main_loop_t);
BELLE_SIP_DECLARE_VPTR(belle_sip_source_t);
BELLE_SIP_DECLARE_VPTR(belle_sip_resolver_context_t);
BELLE_SIP_DECLARE_VPTR(belle_sip_transaction_t);
BELLE_SIP_DECLARE_VPTR(belle_sip_server_transaction_t);
BELLE_SIP_DECLARE_VPTR(belle_sip_client_transaction_t);
BELLE_SIP_DECLARE_VPTR(belle_sip_dialog_t);
BELLE_SIP_DECLARE_VPTR(belle_sip_header_address_t);
BELLE_SIP_DECLARE_VPTR(belle_sip_header_contact_t);
BELLE_SIP_DECLARE_VPTR(belle_sip_header_from_t);
BELLE_SIP_DECLARE_VPTR(belle_sip_header_to_t);
BELLE_SIP_DECLARE_VPTR(belle_sip_header_via_t);
BELLE_SIP_DECLARE_VPTR(belle_sip_uri_t);
BELLE_SIP_DECLARE_VPTR(belle_sip_message_t);
BELLE_SIP_DECLARE_VPTR(belle_sip_request_t);
BELLE_SIP_DECLARE_VPTR(belle_sip_response_t);
BELLE_SIP_DECLARE_VPTR(belle_sip_parameters_t);
BELLE_SIP_DECLARE_VPTR(belle_sip_header_call_id_t);
BELLE_SIP_DECLARE_VPTR(belle_sip_header_cseq_t);
BELLE_SIP_DECLARE_VPTR(belle_sip_header_content_type_t);
BELLE_SIP_DECLARE_VPTR(belle_sip_header_route_t);
BELLE_SIP_DECLARE_VPTR(belle_sip_header_record_route_t);
BELLE_SIP_DECLARE_VPTR(belle_sip_header_user_agent_t);
BELLE_SIP_DECLARE_VPTR(belle_sip_header_content_length_t);
BELLE_SIP_DECLARE_VPTR(belle_sip_header_t);
BELLE_SIP_DECLARE_VPTR(belle_sip_header_extension_t);
BELLE_SIP_DECLARE_VPTR(belle_sip_header_authorization_t);
BELLE_SIP_DECLARE_VPTR(belle_sip_header_www_authenticate_t);
BELLE_SIP_DECLARE_VPTR(belle_sip_header_proxy_authorization_t);
BELLE_SIP_DECLARE_VPTR(belle_sip_header_max_forwards_t);
BELLE_SIP_DECLARE_VPTR(belle_sip_header_expires_t);
BELLE_SIP_DECLARE_VPTR(belle_sip_header_allow_t);
BELLE_SIP_DECLARE_VPTR(belle_sdp_attribute_t);
BELLE_SIP_DECLARE_VPTR(belle_sdp_bandwidth_t);
BELLE_SIP_DECLARE_VPTR(belle_sdp_connection_t);
BELLE_SIP_DECLARE_VPTR(belle_sdp_email_t);
BELLE_SIP_DECLARE_VPTR(belle_sdp_info_t);
BELLE_SIP_DECLARE_VPTR(belle_sdp_key_t);
BELLE_SIP_DECLARE_VPTR(belle_sdp_media_t);
BELLE_SIP_DECLARE_VPTR(belle_sdp_media_description_t);
BELLE_SIP_DECLARE_VPTR(belle_sdp_origin_t);
BELLE_SIP_DECLARE_VPTR(belle_sdp_phone_t);
BELLE_SIP_DECLARE_VPTR(belle_sdp_repeate_time_t);
BELLE_SIP_DECLARE_VPTR(belle_sdp_session_description_t);
BELLE_SIP_DECLARE_VPTR(belle_sdp_session_name_t);
BELLE_SIP_DECLARE_VPTR(belle_sdp_time_t);
BELLE_SIP_DECLARE_VPTR(belle_sdp_time_description_t);
BELLE_SIP_DECLARE_VPTR(belle_sdp_uri_t);
BELLE_SIP_DECLARE_VPTR(belle_sdp_version_t);
BELLE_SIP_DECLARE_VPTR(belle_sdp_base_description_t);
BELLE_SIP_DECLARE_VPTR(belle_sdp_mime_parameter_t);



typedef void (*belle_sip_source_remove_callback_t)(belle_sip_source_t *);

struct belle_sip_source{
	belle_sip_object_t base;
	belle_sip_list_t node;
	unsigned long id;
	int fd;
	unsigned int events;
	int timeout;
	void *data;
	uint64_t expire_ms;
	int index; /* index in pollfd table */
	belle_sip_source_func_t notify;
	belle_sip_source_remove_callback_t on_remove;
	unsigned char cancelled;
	unsigned char expired;
};

void belle_sip_fd_source_init(belle_sip_source_t *s, belle_sip_source_func_t func, void *data, int fd, unsigned int events, unsigned int timeout_value_ms);

#define belle_list_next(elem) ((elem)->next)

/* include private headers */
#include "channel.h"


#ifdef __cplusplus
extern "C"{
#endif



#define belle_sip_new(type) (type*)belle_sip_malloc(sizeof(type))
#define belle_sip_new0(type) (type*)belle_sip_malloc0(sizeof(type))
	
belle_sip_list_t *belle_sip_list_new(void *data);
belle_sip_list_t*  belle_sip_list_append_link(belle_sip_list_t* elem,belle_sip_list_t *new_elem);
belle_sip_list_t *belle_sip_list_find_custom(belle_sip_list_t *list, belle_sip_compare_func compare_func, const void *user_data);
belle_sip_list_t *belle_sip_list_delete_custom(belle_sip_list_t *list, belle_sip_compare_func compare_func, const void *user_data);
belle_sip_list_t * belle_sip_list_free(belle_sip_list_t *list);

#define belle_sip_list_next(elem) ((elem)->next)

extern belle_sip_log_function_t belle_sip_logv_out;

extern unsigned int __belle_sip_log_mask;

#define belle_sip_log_level_enabled(level)   (__belle_sip_log_mask & (level))

#if !defined(WIN32) && !defined(_WIN32_WCE)
#define belle_sip_logv(level,fmt,args) \
{\
        if (belle_sip_logv_out!=NULL && belle_sip_log_level_enabled(level)) \
                belle_sip_logv_out(level,fmt,args);\
        if ((level)==BELLE_SIP_FATAL) abort();\
}while(0)
#else
void belle_sip_logv(int level, const char *fmt, va_list args);
#endif


#ifdef BELLE_SIP_DEBUG_MODE
static inline void belle_sip_debug(const char *fmt,...)
{
  va_list args;
  va_start (args, fmt);
  belle_sip_logv(BELLE_SIP_DEBUG, fmt, args);
  va_end (args);
}
#else

#define belle_sip_debug(...)

#endif

#ifdef BELLE_SIP_NOMESSAGE_MODE

#define belle_sip_log(...)
#define belle_sip_message(...)
#define belle_sip_warning(...)

#else

static inline void belle_sip_log(belle_sip_log_level lev, const char *fmt,...){
        va_list args;
        va_start (args, fmt);
        belle_sip_logv(lev, fmt, args);
        va_end (args);
}

static inline void belle_sip_message(const char *fmt,...)
{
        va_list args;
        va_start (args, fmt);
        belle_sip_logv(BELLE_SIP_MESSAGE, fmt, args);
        va_end (args);
}

static inline void belle_sip_warning(const char *fmt,...)
{
        va_list args;
        va_start (args, fmt);
        belle_sip_logv(BELLE_SIP_WARNING, fmt, args);
        va_end (args);
}

#endif

static inline void belle_sip_error(const char *fmt,...)
{
        va_list args;
        va_start (args, fmt);
        belle_sip_logv(BELLE_SIP_ERROR, fmt, args);
        va_end (args);
}

static inline void belle_sip_fatal(const char *fmt,...)
{
        va_list args;
        va_start (args, fmt);
        belle_sip_logv(BELLE_SIP_FATAL, fmt, args);
        va_end (args);
}





#undef MIN
#define MIN(a,b)	((a)>(b) ? (b) : (a))
#undef MAX
#define MAX(a,b)	((a)>(b) ? (a) : (b))


char * belle_sip_concat (const char *str, ...);

uint64_t belle_sip_time_ms(void);

unsigned int belle_sip_random(void);

char *belle_sip_strdup_printf(const char *fmt,...);


/*parameters accessors*/
#define GET_SET_STRING(object_type,attribute) \
	const char* object_type##_get_##attribute (const object_type##_t* obj) {\
		return obj->attribute;\
	}\
	void object_type##_set_##attribute (object_type##_t* obj,const char* value) {\
		if (obj->attribute != NULL) free((void*)obj->attribute);\
		obj->attribute=malloc(strlen(value)+1);\
		strcpy((char*)(obj->attribute),value);\
	}
#define GET_SET_STRING_PARAM(object_type,attribute) GET_SET_STRING_PARAM2(object_type,attribute,attribute)
#define GET_SET_STRING_PARAM2(object_type,attribute,func_name) \
	const char* object_type##_get_##func_name (const object_type##_t* obj) {\
	const char* l_value = belle_sip_parameters_get_parameter(BELLE_SIP_PARAMETERS(obj),#attribute);\
	if (l_value == NULL) { \
		/*belle_sip_warning("cannot find parameters [%s]",#attribute);*/\
		return NULL;\
	}\
	return l_value;\
	}\
	void object_type##_set_##func_name (object_type##_t* obj,const char* value) {\
		belle_sip_parameters_set_parameter(BELLE_SIP_PARAMETERS(obj),#attribute,value);\
	}

#define DESTROY_STRING(object,attribute) if (object->attribute) belle_sip_free((void*)object->attribute);

#define CLONE_STRING(object_type,attribute,dest,src) \
		if ( object_type##_get_##attribute (src)) {\
			object_type##_set_##attribute(dest,object_type##_get_##attribute(src));\
		}

#define GET_SET_INT(object_type,attribute,type) GET_SET_INT_PRIVATE(object_type,attribute,type,)

#define GET_SET_INT_PRIVATE(object_type,attribute,type,set_prefix) \
	type  object_type##_get_##attribute (const object_type##_t* obj) {\
		return obj->attribute;\
	}\
	void set_prefix##object_type##_set_##attribute (object_type##_t* obj,type  value) {\
		obj->attribute=value;\
	}
#define GET_SET_INT_PARAM(object_type,attribute,type) GET_SET_INT_PARAM_PRIVATE(object_type,attribute,type,)
#define GET_SET_INT_PARAM2(object_type,attribute,type,func_name) GET_SET_INT_PARAM_PRIVATE2(object_type,attribute,type,,func_name)

#define ATO_(type,value) ATO_##type(value)
#define ATO_int(value) atoi(value)
#define ATO_float(value) strtof(value,NULL)
#define FORMAT_(type) FORMAT_##type
#define FORMAT_int    "%i"
#define FORMAT_float  "%f"

#define GET_SET_INT_PARAM_PRIVATE(object_type,attribute,type,set_prefix) GET_SET_INT_PARAM_PRIVATE2(object_type,attribute,type,set_prefix,attribute)
#define GET_SET_INT_PARAM_PRIVATE2(object_type,attribute,type,set_prefix,func_name) \
	type  object_type##_get_##func_name (const object_type##_t* obj) {\
		const char* l_value = belle_sip_parameters_get_parameter(BELLE_SIP_PARAMETERS(obj),#attribute);\
		if (l_value == NULL) { \
			belle_sip_error("cannot find parameters [%s]",#attribute);\
			return -1;\
		}\
		return ATO_(type,l_value);\
	}\
	void set_prefix##object_type##_set_##func_name (object_type##_t* obj,type  value) {\
		char l_str_value[16];\
		if (value == -1) { \
			belle_sip_parameters_remove_parameter(BELLE_SIP_PARAMETERS(obj),#attribute);\
			return;\
		}\
		snprintf(l_str_value,16,FORMAT_(type),value);\
		belle_sip_parameters_set_parameter(BELLE_SIP_PARAMETERS(obj),#attribute,(const char*)l_str_value);\
	}

#define GET_SET_BOOL(object_type,attribute,getter) \
	unsigned int object_type##_##getter##_##attribute (const object_type##_t* obj) {\
		return obj->attribute;\
	}\
	void object_type##_set_##attribute (object_type##_t* obj,unsigned int value) {\
		obj->attribute=value;\
	}
#define GET_SET_BOOL_PARAM2(object_type,attribute,getter,func_name) \
	unsigned int object_type##_##getter##_##func_name (const object_type##_t* obj) {\
		return belle_sip_parameters_is_parameter(BELLE_SIP_PARAMETERS(obj),#attribute);\
	}\
	void object_type##_set_##func_name (object_type##_t* obj,unsigned int value) {\
		belle_sip_parameters_set_parameter(BELLE_SIP_PARAMETERS(obj),#attribute,NULL);\
	}

#define BELLE_SIP_PARSE(object_type) \
belle_sip_##object_type##_t* belle_sip_##object_type##_parse (const char* value) { \
	pANTLR3_INPUT_STREAM           input; \
	pbelle_sip_messageLexer               lex; \
	pANTLR3_COMMON_TOKEN_STREAM    tokens; \
	pbelle_sip_messageParser              parser; \
	input  = antlr3NewAsciiStringCopyStream	(\
			(pANTLR3_UINT8)value,\
			(ANTLR3_UINT32)strlen(value),\
			NULL);\
	lex    = belle_sip_messageLexerNew                (input);\
	tokens = antlr3CommonTokenStreamSourceNew  (ANTLR3_SIZE_HINT, TOKENSOURCE(lex));\
	parser = belle_sip_messageParserNew               (tokens);\
	belle_sip_##object_type##_t* l_parsed_object = parser->object_type(parser);\
	parser ->free(parser);\
	tokens ->free(tokens);\
	lex    ->free(lex);\
	input  ->close(input);\
	if (l_parsed_object == NULL) belle_sip_error(#object_type" parser error for [%s]",value);\
	return l_parsed_object;\
}

#define BELLE_SIP_NEW(object_type,super_type) BELLE_SIP_NEW_HEADER(object_type,super_type,NULL)
#define BELLE_SIP_NEW_HEADER(object_type,super_type,name) BELLE_SIP_NEW_HEADER_INIT(object_type,super_type,name,header)
#define BELLE_SIP_NEW_HEADER_INIT(object_type,super_type,name,init_type) \
		BELLE_SIP_DECLARE_NO_IMPLEMENTED_INTERFACES(belle_sip_##object_type##_t); \
		BELLE_SIP_INSTANCIATE_VPTR(	belle_sip_##object_type##_t\
									, belle_sip_##super_type##_t\
									, belle_sip_##object_type##_destroy\
									, belle_sip_##object_type##_clone\
									, belle_sip_##object_type##_marshal, TRUE); \
		belle_sip_##object_type##_t* belle_sip_##object_type##_new () { \
		belle_sip_##object_type##_t* l_object = belle_sip_object_new(belle_sip_##object_type##_t);\
		belle_sip_##super_type##_init((belle_sip_##super_type##_t*)l_object); \
		belle_sip_##init_type##_init((belle_sip_##init_type##_t*) l_object); \
		if (name) belle_sip_header_set_name(BELLE_SIP_HEADER(l_object),name);\
		return l_object;\
	}
typedef struct belle_sip_param_pair_t {
	int ref;
	char* name;
	char* value;
} belle_sip_param_pair_t;


belle_sip_param_pair_t* belle_sip_param_pair_new(const char* name,const char* value);

void belle_sip_param_pair_destroy(belle_sip_param_pair_t*  pair) ;

int belle_sip_param_pair_comp_func(const belle_sip_param_pair_t *a, const char*b) ;

belle_sip_param_pair_t* belle_sip_param_pair_ref(belle_sip_param_pair_t* obj);

void belle_sip_param_pair_unref(belle_sip_param_pair_t* obj);


void belle_sip_header_address_set_quoted_displayname(belle_sip_header_address_t* address,const char* value);

/*calss header*/
struct _belle_sip_header {
	belle_sip_object_t base;
	belle_sip_header_t* next;
	const char* name;
};

void belle_sip_header_set_next(belle_sip_header_t* header,belle_sip_header_t* next);
belle_sip_header_t* belle_sip_header_get_next(const belle_sip_header_t* headers);

void belle_sip_header_init(belle_sip_header_t* obj);
/*class parameters*/
struct _belle_sip_parameters {
	belle_sip_header_t base;
	belle_sip_list_t* param_list;
	belle_sip_list_t* paramnames_list;
};

void belle_sip_parameters_init(belle_sip_parameters_t *obj);

/*
 * Listening points
*/

typedef struct belle_sip_udp_listening_point belle_sip_udp_listening_point_t;

BELLE_SIP_DECLARE_CUSTOM_VPTR_BEGIN(belle_sip_listening_point_t,belle_sip_object_t)
const char *transport;
belle_sip_channel_t * (*create_channel)(belle_sip_listening_point_t *,const char *dest_ip, int port);
BELLE_SIP_DECLARE_CUSTOM_VPTR_END

BELLE_SIP_DECLARE_CUSTOM_VPTR_BEGIN(belle_sip_udp_listening_point_t,belle_sip_listening_point_t)
BELLE_SIP_DECLARE_CUSTOM_VPTR_END

#define BELLE_SIP_LISTENING_POINT(obj) BELLE_SIP_CAST(obj,belle_sip_listening_point_t)
#define BELLE_SIP_UDP_LISTENING_POINT(obj) BELLE_SIP_CAST(obj,belle_sip_udp_listening_point_t)

belle_sip_listening_point_t * belle_sip_udp_listening_point_new(belle_sip_stack_t *s, const char *ipaddress, int port);
belle_sip_channel_t *belle_sip_listening_point_get_channel(belle_sip_listening_point_t *ip, const char *dest, int port);
belle_sip_channel_t *belle_sip_listening_point_create_channel(belle_sip_listening_point_t *ip, const char *dest, int port);
int belle_sip_listening_point_get_well_known_port(const char *transport);


/*
 belle_sip_stack_t
*/
struct belle_sip_stack{
	belle_sip_object_t base;
	belle_sip_main_loop_t *ml;
	belle_sip_list_t *lp;/*list of listening points*/
	belle_sip_timer_config_t timer_config;
};

void belle_sip_stack_get_next_hop(belle_sip_stack_t *stack, belle_sip_request_t *req, belle_sip_hop_t *hop);

const belle_sip_timer_config_t *belle_sip_stack_get_timer_config(const belle_sip_stack_t *stack);

/*
 belle_sip_provider_t
*/

struct belle_sip_provider{
	belle_sip_object_t base;
	belle_sip_stack_t *stack;
	belle_sip_list_t *lps; /*listening points*/
	belle_sip_list_t *listeners;
};

belle_sip_provider_t *belle_sip_provider_new(belle_sip_stack_t *s, belle_sip_listening_point_t *lp);
void belle_sip_provider_set_transaction_terminated(belle_sip_provider_t *p, belle_sip_transaction_t *t);
belle_sip_channel_t * belle_sip_provider_get_channel(belle_sip_provider_t *p, const char *name, int port, const char *transport);

typedef struct listener_ctx{
	belle_sip_listener_t *listener;
	void *data;
}listener_ctx_t;

#define BELLE_SIP_PROVIDER_INVOKE_LISTENERS(provider,callback,event) \
	BELLE_SIP_INVOKE_LISTENERS_ARG(((provider)->listeners),belle_sip_listener_t,callback,(event))


/*
 belle_sip_transaction_t
*/

struct belle_sip_transaction{
	belle_sip_object_t base;
	belle_sip_provider_t *provider; /*the provider that created this transaction */
	belle_sip_request_t *request;
	belle_sip_response_t *prov_response;
	belle_sip_response_t *final_response;
	char *branch_id;
	belle_sip_transaction_state_t state;
	uint64_t start_time;
	belle_sip_source_t *timer;
	int interval;
	int is_reliable:1;
	int is_server:1;
	int is_invite:1;
	void *appdata;
};


belle_sip_client_transaction_t * belle_sip_client_transaction_new(belle_sip_provider_t *prov,belle_sip_request_t *req);
belle_sip_server_transaction_t * belle_sip_server_transaction_new(belle_sip_provider_t *prov,belle_sip_request_t *req);
void belle_sip_client_transaction_add_response(belle_sip_client_transaction_t *t, belle_sip_response_t *resp);

/*
 belle_sip_response_t
*/
void belle_sip_response_get_return_hop(belle_sip_response_t *msg, belle_sip_hop_t *hop);

#define IS_TOKEN(token) \
		(INPUT->toStringTT(INPUT,LT(1),LT(strlen(#token)))->chars ?\
		strcmp(#token,(const char*)(INPUT->toStringTT(INPUT,LT(1),LT(strlen(#token)))->chars)) == 0:0)
char* _belle_sip_str_dup_and_unquote_string(const char* quoted_string);

/*********************************************************
 * SDP
 */
#define BELLE_SDP_PARSE(object_type) \
belle_sdp_##object_type##_t* belle_sdp_##object_type##_parse (const char* value) { \
	pANTLR3_INPUT_STREAM           input; \
	pbelle_sdpLexer               lex; \
	pANTLR3_COMMON_TOKEN_STREAM    tokens; \
	pbelle_sdpParser              parser; \
	input  = antlr3NewAsciiStringCopyStream	(\
			(pANTLR3_UINT8)value,\
			(ANTLR3_UINT32)strlen(value),\
			NULL);\
	lex    = belle_sdpLexerNew                (input);\
	tokens = antlr3CommonTokenStreamSourceNew  (ANTLR3_SIZE_HINT, TOKENSOURCE(lex));\
	parser = belle_sdpParserNew               (tokens);\
	belle_sdp_##object_type##_t* l_parsed_object = parser->object_type(parser).ret;\
	parser ->free(parser);\
	tokens ->free(tokens);\
	lex    ->free(lex);\
	input  ->close(input);\
	if (l_parsed_object == NULL) belle_sip_error(#object_type" parser error for [%s]",value);\
	return l_parsed_object;\
}
#define BELLE_SDP_NEW(object_type,super_type) \
		BELLE_SIP_DECLARE_NO_IMPLEMENTED_INTERFACES(belle_sdp_##object_type##_t); \
		BELLE_SIP_INSTANCIATE_VPTR(	belle_sdp_##object_type##_t\
									, super_type##_t\
									, belle_sdp_##object_type##_destroy\
									, belle_sdp_##object_type##_clone\
									, belle_sdp_##object_type##_marshal, TRUE); \
		belle_sdp_##object_type##_t* belle_sdp_##object_type##_new () { \
		belle_sdp_##object_type##_t* l_object = belle_sip_object_new(belle_sdp_##object_type##_t);\
		super_type##_init((super_type##_t*)l_object); \
		return l_object;\
	}
#define BELLE_SDP_NEW_WITH_CTR(object_type,super_type) \
		BELLE_SIP_DECLARE_NO_IMPLEMENTED_INTERFACES(belle_sdp_##object_type##_t); \
		BELLE_SIP_INSTANCIATE_VPTR(	belle_sdp_##object_type##_t\
									, super_type##_t\
									, belle_sdp_##object_type##_destroy\
									, belle_sdp_##object_type##_clone\
									, belle_sdp_##object_type##_marshal,TRUE); \
		belle_sdp_##object_type##_t* belle_sdp_##object_type##_new () { \
		belle_sdp_##object_type##_t* l_object = belle_sip_object_new(belle_sdp_##object_type##_t);\
		super_type##_init((super_type##_t*)l_object); \
		belle_sdp_##object_type##_init(l_object); \
		return l_object;\
	}





#ifdef __cplusplus
}
#endif


/*include private headers */
#include "belle_sip_resolver.h"

#define BELLE_SIP_SOCKET_TIMEOUT 30000

#endif
