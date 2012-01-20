/*
 ============================================================================
 Name        : hev-scgi-handler.h
 Author      : Heiher <admin@heiher.info>
 Version     : 0.0.1
 Copyright   : Copyright (C) 2011 everyone.
 Description : 
 ============================================================================
 */

#ifndef __HEV_SCGI_HANDLER_H__
#define __HEV_SCGI_HANDLER_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define HEV_TYPE_SCGI_HANDLER			(hev_scgi_handler_get_type())
#define HEV_SCGI_HANDLER(obj)			(G_TYPE_CHECK_INSTANCE_CAST((obj), \
									HEV_TYPE_SCGI_HANDLER, HevSCGIHandler))
#define HEV_IS_SCGI_HANDLER(obj)		(G_TYPE_CHECK_INSTANCE_TYPE((obj), \
									HEV_TYPE_SCGI_HANDLER))
#define HEV_SCGI_HANDLER_GET_INTERFACE(inst)	(G_TYPE_INSTANCE_GET_INTERFACE((inst), \
											HEV_TYPE_SCGI_HANDLER, HevSCGIHandlerInterface))

typedef struct _HevSCGIHandler HevSCGIHandler;
typedef struct _HevSCGIHandlerInterface HevSCGIHandlerInterface;

struct _HevSCGIHandlerInterface
{
	GTypeInterface parent_scgi_handler;

	const gchar * (*get_name)(HevSCGIHandler *self);
	const gchar * (*get_version)(HevSCGIHandler *self);
	const gchar * (*get_pattern)(HevSCGIHandler *self);
	void (*handle)(HevSCGIHandler *self, GObject *scgi_task);
};

GType hev_scgi_handler_get_type(void);

const gchar * hev_scgi_handler_get_name(HevSCGIHandler *self);
const gchar * hev_scgi_handler_get_version(HevSCGIHandler *self);
const gchar * hev_scgi_handler_get_pattern(HevSCGIHandler *self);
void hev_scgi_handler_handle(HevSCGIHandler *self, GObject *scgi_task);

G_END_DECLS

#endif /* __HEV_SCGI_HANDLER_H__ */

