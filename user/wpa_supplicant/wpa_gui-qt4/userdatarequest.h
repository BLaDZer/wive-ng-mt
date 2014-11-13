/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
/*
 * wpa_gui - UserDataRequest class
 * Copyright (c) 2005-2006, Jouni Malinen <jkmaline@cc.hut.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 */

#ifndef USERDATAREQUEST_H
#define USERDATAREQUEST_H

#include <QObject>
#include "ui_userdatarequest.h"

class WpaGui;

class UserDataRequest : public QDialog, public Ui::UserDataRequest
{
	Q_OBJECT

public:
	UserDataRequest(QWidget *parent = 0, const char *name = 0,
			bool modal = false, Qt::WFlags fl = 0);
	~UserDataRequest();

	int setParams(WpaGui *_wpagui, const char *reqMsg);

public slots:
	virtual void sendReply();

protected slots:
	virtual void languageChange();

private:
	WpaGui *wpagui;
	int networkid;
	QString field;
};

#endif /* USERDATAREQUEST_H */
