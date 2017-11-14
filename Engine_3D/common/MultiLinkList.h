#pragma once

#include "../flatform/EPaint.h"

template <typename T>
class MultiLinkList
{
public:
	MultiLinkList(INT linkindex);
	~MultiLinkList();

	T * link;
	INT linkcount;
	INT linkindex;

	VOID insertLink(T * link, T * before = NULL, T * after = NULL);
	VOID insertSort(T * link);
	T * isin(T * link);
	T * removeLink(T * link);
	T * getLink(INT uniqueID);
	T * getPos(INT pos);
	T * next(T * link);
	T * prev(T * link);
	VOID clearLink();
};

template<typename T>
MultiLinkList<T>::MultiLinkList(INT linkindex) :
linkcount(0),
link(NULL),
linkindex(linkindex)
{
}

template<typename T>
MultiLinkList<T>::~MultiLinkList()
{
	if (this->link)
	{
		T * temp = this->link;
		do
		{
			if (removeLink(temp) == NULL)
			{
				break;
			}

			delete temp;

			temp = this->link;
		} while (temp);
	}
}

template<typename T>
VOID MultiLinkList<T>::clearLink()
{
	if (this->link)
	{
		T * temp = this->link;
		do
		{
			if (removeLink(temp) == NULL)
			{
				break;
			}

			temp = this->link;
		} while (temp);
	}
}

template<typename T>
VOID MultiLinkList<T>::insertSort(T * link)
{
	if (link == NULL)
	{
		return;
	}

	if (this->link)
	{
		T * temp = this->link;
		do
		{
			if (temp->f > link->f)
			{
				this->insertLink(link, temp);
				return;
			}

			temp = temp->next[this->linkindex];
		} while (temp && temp != this->link);
		this->insertLink(link);
	}
	else
	{
		this->insertLink(link);
	}
}

template<typename T>
VOID MultiLinkList<T>::insertLink(T * link, T * before, T * after)
{
	if (link == NULL)
	{
		return;
	}
	if (this->link == NULL)
	{
		this->link = link;

		this->link->prev[this->linkindex] = link;
		this->link->next[this->linkindex] = link;

		this->linkcount = this->linkcount + 1;

		return;
	}
	else
	{
		T * _link = NULL;
		if (before == this->link)
		{
			_link = link;
		}
		if (before == NULL && after == NULL)
		{
			before = this->link;
			after = this->link->prev[this->linkindex];
		}
		else if (before == NULL)
		{
			before = after->next[this->linkindex];
		}
		else if (after == NULL)
		{
			after = before->prev[this->linkindex];
		}
		else /* before != NULL && after != NULL*/
		{
			if (before->prev[this->linkindex] != after || after->next[this->linkindex] != before)
			{
				return;
			}
		}
		if (before == NULL || after == NULL ||
			before->prev[this->linkindex] == NULL ||
			after->next[this->linkindex] == NULL)
		{
			return;
		}

		link->prev[this->linkindex] = after;
		link->next[this->linkindex] = before;
		after->next[this->linkindex] = link;
		before->prev[this->linkindex] = link;

		if (_link)
		{
			this->link = _link;
		}

		this->linkcount = this->linkcount + 1;
	}
}

template<typename T>
T * MultiLinkList<T>::removeLink(T * link)
{
	if (link == NULL)
	{
		return NULL;
	}
	if (this->linkindex < 0)
	{
		return NULL;
	}
	if (link->prev[this->linkindex] == NULL || link->next[this->linkindex] == NULL)
	{
		return NULL;
	}
	T * before = link->prev[this->linkindex];
	T * after = link->next[this->linkindex];

	before->next[this->linkindex] = after;
	after->prev[this->linkindex] = before;
	link->prev[this->linkindex] = NULL;
	link->next[this->linkindex] = NULL;

	if (this->link == link)
	{
		this->link = after;
	}
	if (this->link == link)
	{
		this->link = NULL;
	}

	this->linkcount = this->linkcount - 1;

	return link;
}

template<typename T>
T * MultiLinkList<T>::getLink(INT uniqueID)
{
	if (this->link == NULL)
	{
		return NULL;
	}
	T * temp = this->link;
	do
	{
		if (temp->uniqueID == uniqueID)
		{
			return temp;
		}
		temp = temp->next[this->linkindex];
	} while (temp && temp != this->link);
	return NULL;
}

template<typename T>
T * MultiLinkList<T>::getPos(INT pos)
{
	if (this->link == NULL)
	{
		return NULL;
	}
	T * temp = this->link;
	INT i = 0;
	do
	{
		if (i++ == pos) {
			return temp;
		}

		temp = temp->next[this->linkindex];
	} while (temp && temp != this->link);
	return NULL;
}

template<typename T>
T * MultiLinkList<T>::isin(T * link)
{
	if (this->link == NULL)
	{
		return NULL;
	}
	T * temp = this->link;
	do
	{
		if (temp->isin(link))
		{
			return temp;
		}
		temp = temp->next[this->linkindex];
	} while (temp && temp != this->link);
	return NULL;
}

template<typename T>
T * MultiLinkList<T>::next(T * link)
{
	if (link == NULL)
	{
		return NULL;
	}
	return link->next[this->linkindex];
}

template<typename T>
T * MultiLinkList<T>::prev(T * link)
{
	if (link == NULL)
	{
		return NULL;
	}
	return link->prev[this->linkindex];
}