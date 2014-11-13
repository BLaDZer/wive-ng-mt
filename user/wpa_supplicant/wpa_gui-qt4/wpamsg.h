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
 * wpa_gui - WpaMsg class for storing event messages
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

#ifndef WPAMSG_H
#define WPAMSG_H

#include <QDateTime>
#include <QLinkedList>

class WpaMsg {
public:
	WpaMsg() {}
	WpaMsg(const QString &_msg, int _priority = 2)
		: msg(_msg), priority(_priority)
	{
		timestamp = QDateTime::currentDateTime();
	}

	QString getMsg() const { return msg; }
	int getPriority() const { return priority; }
	QDateTime getTimestamp() const { return timestamp; }

private:
	QString msg;
	int priority;
	QDateTime timestamp;
};

typedef QLinkedList<WpaMsg> WpaMsgList;

#endif /* WPAMSG_H */
