#ifndef CRPC_TRIE_H_
#define CRPC_TRIE_H_

#include <unordered_map>

template <typename T, typename Value>
class Trie
{
public:
    struct TrieNode
    {
        //是否是一个节点
        bool is_word;
        Value value;
        std::unordered_map<T, TrieNode*> trie_map;
    };

    Trie()
    {}

    void insert(const T* buff, size_t size, const Value& value)
    {
        if (!size)
            return;

        TrieNode* cur = &_root;
        for (size_t i = 0; i < size; ++i)
        {
            if (cur->trie_map[buff[i]] == NULL)
            {
                cur->trie_map[buff[i]] = new TrieNode();
            }

            cur = cur->trie_map[buff[i]];
        }
        cur->is_word = true;
        cur->value = value;
    }

    bool find_prefix(const T* buff, size_t size, Value& ret)
    {
        bool found = false;
        TrieNode* cur = &_root;
        for (size_t i = 0; i < size; ++i)
        {
            auto itr = cur->trie_map.find(buff[i]);
            if (itr == cur->trie_map.end())
                return found;

            cur = cur->trie_map[buff[i]];
            if (cur->is_word)
            {
                found = true;
                ret = cur->value;
            }
        }
        return found;
    }

    void remove(const T* buff, size_t size)
    {
        if (!size)
            return;

        size_t last_word_pos = 0;
        TrieNode* last_word, *cur;
        last_word = cur = &_root;

        for (size_t i = 0; i < size; ++i)
        {
            if (cur->trie_map.find(buff[i]) == cur->trie_map.end())
            {
                return;
            }

            TrieNode* pre = cur;

            cur = cur->trie_map[buff[i]];
            if (cur->is_word || cur->trie_map.size() > 1)
            {
                last_word_pos = i;
                last_word = pre;
            }
        }


        //123445
        //123

        //check valid
        if (!cur || !cur->is_word || cur->trie_map.size())
            return;

        cur = last_word->trie_map[buff[last_word_pos]];
        last_word->trie_map.erase(buff[last_word_pos]);

        remove_internal(cur);
    }

private:
    void remove_internal(TrieNode* cur)
    {
        if (!cur)
            return;

        while (cur->trie_map.size())
        {
            auto itr = cur->trie_map.begin();
            remove_internal(itr->second);
            cur->trie_map.erase(itr->first);
        }
        delete cur;
    }

    TrieNode _root;
};

#endif
