#include <LckFreeLst.h>
#include <Constants.h>

template<typename KeyType, typename ValType>
Node<KeyType, ValType>::Node():next(NULL), marked(false)
{

}

template<typename KeyType, typename ValType>
Node<KeyType, ValType>::~Node()
{

}

template<typename KeyType, typename ValType>
Node<KeyType, ValType>::Node(KeyType k, ValType v):key(k), val(v), next(NULL),  marked(false)
{
}

template<typename KeyType, typename ValType>
LckFreeLst<KeyType, ValType>::LckFreeLst()
{
	head = new Node<KeyType, ValType> ();
	tail = new Node<KeyType, ValType> ();
	head->next = tail;

}

template<typename KeyType, typename ValType>
bool
LckFreeLst<KeyType, ValType>::erase(KeyType search_key)
{
	Node<KeyType, ValType> *right_node, *right_node_next, *left_node;
	do 
	{
		right_node = search (search_key, &left_node);
		if ((right_node == tail) || (right_node->key != search_key))
			return false;
		right_node_next = right_node->next;
		if (!is_marked_reference(right_node))
			if (__sync_bool_compare_and_swap (&(right_node->marked), right_node->marked, true))
			{
				std::lock_guard<std::mutex> lock(gcmutex);
				garbageCollector[time(NULL)] = right_node;
				break;
			}
	} 
	while (true);
	if (!__sync_bool_compare_and_swap (&(left_node->next), right_node, right_node_next))
		right_node = search (right_node->key, &left_node);
	else
		delete right_node;
	return true;
}

template<typename KeyType, typename ValType>
bool
LckFreeLst<KeyType, ValType>::insert(KeyType key, ValType val)
{
	Node<KeyType, ValType> *new_node = new Node<KeyType, ValType>(key, val);
	Node<KeyType, ValType> *right_node, *left_node;

	do 
	{
		right_node = search (key, &left_node);
		if ((right_node != tail) && (right_node->key == key)) 
		{
			delete new_node;
			return false;
		}
		new_node->next = right_node;
		if (__sync_bool_compare_and_swap (&(left_node->next), right_node, new_node))
			return true;
	} 
	while (true); 
}

template<typename KeyType, typename ValType>
bool
LckFreeLst<KeyType, ValType>::insertOrUpdate(KeyType key, ValType val)
{
	Node<KeyType, ValType> *new_node = new Node<KeyType, ValType>(key, val);
	Node<KeyType, ValType> *right_node, *left_node;

	do 
	{
		right_node = search (key, &left_node);
		if ((right_node != tail) && (right_node->key == key)) 
		{
			delete new_node;
			right_node->val = val;
			return false;
		}
		new_node->next = right_node;
		if (__sync_bool_compare_and_swap (&(left_node->next), right_node, new_node))
			return true;
	} 
	while (true); 
}

template<typename KeyType, typename ValType>
bool
LckFreeLst<KeyType, ValType>::find(KeyType key)
{
	Node<KeyType, ValType> *right_node, *left_node;
	
	right_node = search(key, &left_node);
	if (right_node == tail || right_node->key != key)
		return false;
	else
		return true;
}


template<typename KeyType, typename ValType>
Node<KeyType, ValType> *
LckFreeLst<KeyType, ValType>::search (KeyType search_key, Node<KeyType, ValType> **left_node)
{
	Node<KeyType, ValType> *left_node_next, *right_node;
    search_again:
        do 
	{
    		Node<KeyType, ValType> *t = head;
    		Node<KeyType, ValType> *t_next = head->next;
		/* 1: Find left_node and right_node */ 
		do 
		{
			if (!is_marked_reference(t)) 
			{ 
				(*left_node) = t;
				left_node_next = t_next;
      			}
      			t = t_next;
			//t->marked = false;
      			if (t == tail) 
				break;
      			t_next = t->next;
		} 
		while (is_marked_reference(t) || (t->key < search_key)); 
		right_node = t;
		/* 2: Check nodes are adjacent */ 
		if (left_node_next == right_node)
			if ((right_node != tail) && is_marked_reference(right_node)) 
				goto search_again;
			else
				return right_node;
		/* 3: Remove one or more marked nodes */
		if (__sync_bool_compare_and_swap (&((*left_node)->next), left_node_next, right_node))
			if ((right_node != tail) && is_marked_reference(right_node)) 
				goto search_again;
			else
			{
				return right_node;
			}
	} 
	while (true);
	return NULL;
}

template<typename KeyType, typename ValType>
bool
LckFreeLst<KeyType, ValType>::is_marked_reference(Node<KeyType, ValType> *pNode)
{
	return pNode->is_marked_reference();
}

template<typename KeyType, typename ValType>
LckFreeLst<KeyType, ValType>::~LckFreeLst()
{
	delete head;
	delete tail;
}

template<typename KeyType, typename ValType>
bool
LckFreeLst<KeyType, ValType>::isEmpty()
{
	if (head->next == tail)
	{
		return true;
	}
	return false;
}

template<typename KeyType, typename ValType>
void
LckFreeLst<KeyType, ValType>::cleanUp()
{
	time_t now = time(NULL);
	std::lock_guard<std::mutex> lock(gcmutex);
	for (auto pItr = garbageCollector.begin(); pItr != garbageCollector.end();  )
	{
		double diff = difftime(now, pItr->first);	
		if (diff > LCKFREE_LST_TDIFF)
		{
			delete pItr->second;
			garbageCollector.erase(pItr++);	
		}
		else
		{
			++pItr;
		}
	}
	return;
}


template<typename KeyType, typename ValType>
void
LckFreeLst<KeyType, ValType>::getVals(std::vector<ValType>& vals) const
{
	auto pItr = head;
	while (pItr != NULL)
	{
		vals.push_back(pItr->val);	
		pItr = pItr->next;
	}
	return;
}

template<typename KeyType, typename ValType>
void
LckFreeLst<KeyType, ValType>::getKeys(std::vector<KeyType>& keys) const
{
	auto pItr = head;
	while (pItr != NULL)
	{
		keys.push_back(pItr->key);	
		pItr = pItr->next;
	}
	return;
}

template<typename KeyType, typename ValType>
void
LckFreeLst<KeyType, ValType>::getKeyVals(std::map<KeyType, ValType>& kvals) const
{
	auto pItr = head;
	while (pItr != NULL)
	{
		kvals[pItr->key] = pItr->val;
		pItr = pItr->next;
	}
	return;
}

template<typename KeyType, typename ValType>
bool
LckFreeLst<KeyType, ValType>::getVal(const KeyType& key, ValType& val) const
{

	auto pItr = head;
	while (pItr != NULL)
	{
		if (key == pItr->key)
		{
			val = pItr->val;
			return true;
		}
		else if (key > pItr->key)
		{
			return false;
		}
		pItr = pItr->next;
	}
	return false;
}

template class LckFreeLst<std::string, std::string>;
template class LckFreeLst<std::string, timstr>;
template class LckFreeLst<long, std::string>;
