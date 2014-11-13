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
#ifndef WPAMSG_H
#define WPAMSG_H

class WpaMsg;

#if QT_VERSION >= 0x040000
#include <QDateTime>
#include <QLinkedList>
typedef QLinkedList<WpaMsg> WpaMsgList;
#else
#include <qdatetime.h>
typedef QValueList<WpaMsg> WpaMsgList;
#endif

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

#endif /* WPAMSG_H */
