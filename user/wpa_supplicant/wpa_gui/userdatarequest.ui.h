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
/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/

int UserDataRequest::setParams(WpaGui *_wpagui, const char *reqMsg)
{
    char *tmp, *pos, *pos2;
    wpagui = _wpagui;
    tmp = strdup(reqMsg);
    if (tmp == NULL)
	return -1;
    pos = strchr(tmp, '-');
    if (pos == NULL) {
	free(tmp);
	return -1;
    }
    *pos++ = '\0';
    field = tmp;
    pos2 = strchr(pos, ':');
    if (pos2 == NULL) {
	free(tmp);
	return -1;
    }
    *pos2++ = '\0';
    
    networkid = atoi(pos);
    queryInfo->setText(pos2);
    if (strcmp(tmp, "PASSWORD") == 0) {
	queryField->setText("Password: ");
	queryEdit->setEchoMode(QLineEdit::Password);
    } else if (strcmp(tmp, "NEW_PASSWORD") == 0) {
	queryField->setText("New password: ");
 	queryEdit->setEchoMode(QLineEdit::Password);
    } else if (strcmp(tmp, "IDENTITY") == 0)
	queryField->setText("Identity: ");
    else if (strcmp(tmp, "PASSPHRASE") == 0) {
	queryField->setText("Private key passphrase: ");
 	queryEdit->setEchoMode(QLineEdit::Password);
    } else
	queryField->setText(field + ":");
    free(tmp);
    
    return 0;
}


void UserDataRequest::sendReply()
{
    char reply[10];
    size_t reply_len = sizeof(reply);
    
    if (wpagui == NULL) {
	reject();
	return;
    }
    
    QString cmd = QString(WPA_CTRL_RSP) + field + '-' +
		  QString::number(networkid) + ':' +
		  queryEdit->text();
    wpagui->ctrlRequest(cmd.ascii(), reply, &reply_len);
    accept();
}
