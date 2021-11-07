#pragma once
#include <Arduino.h>
namespace JustusDevTools
{
    template <class T>
    class LinkedList
    {
    private:
        class LinkedNode
        {
        public:
            T data;
            LinkedNode *forward;
            LinkedNode *back;

            LinkedNode()
            {
                this->data = NULL;
                this->forward = NULL;
                this->back = NULL;
            }
            LinkedNode(T data)
            {
                this->data = data;
                this->forward = NULL;
                this->back = NULL;
            }
            LinkedNode(LinkedNode *back, T data)
            {
                this->data = data;
                this->back = back;
                this->forward = NULL;
            }
        };

        size_t size;
        LinkedNode *firstNode;
        LinkedNode *currentNode;

    public:
        //inits the list
        LinkedList()
        {
            size = 0;
            firstNode = NULL;
            currentNode = NULL;
        }

        //returns size of the list
        size_t length()
        {
            return size;
        }

        //adds a node to the last of the list
        void add(T data)
        {
            size += 1;
            if (firstNode == NULL)
            {
                currentNode = new LinkedNode(data);
                firstNode = currentNode;
            }
            else
            {
                goto_last();
                LinkedNode *tempNode = new LinkedNode(currentNode, data);
                currentNode->forward = tempNode;
                currentNode = tempNode;
            }
        }

        //removes the current node and preserves the list
        void remove()
        {
            size--;
            if (currentNode->forward == NULL)
            {
                if (currentNode->back != NULL)
                { //forward false | back true
                    currentNode = currentNode->back;
                    delete currentNode->forward;
                }
                else
                    delete currentNode; //forward false | back false
            }
            else
            {
                if (currentNode->back != NULL)
                { //forward true | back true
                    LinkedNode *tempNode = currentNode->forward;
                    currentNode = currentNode->back;
                    delete currentNode->forward;
                    currentNode->forward = tempNode;
                }
                else
                { //forward true | back false
                    currentNode = currentNode->forward;
                    delete currentNode->back;
                    firstNode = currentNode;
                }
            }
        }

        //gets the current data
        T get_data()
        {
            if (currentNode != NULL)
                return currentNode->data;
            else
                return false;
        }

        //gets the current data then increments forward
        T get_next()
        {
            T data;
            bool exists = false;
            if (currentNode != NULL)
            {
                exists = true;
                data = currentNode->data;
            }
            currentNode = currentNode->forward;
            if (exists)
            {
                return data;
            }
            else
                return false;
        }

        //gets the current data then increments back
        T get_back()
        {
            T data;
            bool exists = false;
            if (currentNode != NULL)
            {
                exists = true;
                data = currentNode->data;
            }
            currentNode = currentNode->back;
            if (exists)
            {
                return data;
            }
            else
                return false;
        }

        //goes to the first node
        void goto_first()
        {
            currentNode = firstNode;
        }

        //goes to the last node
        void goto_last()
        {
            while (currentNode->forward != NULL)
            {
                currentNode = currentNode->forward;
            }
        }



        //moves 1 forward
        void move_forward()
        {
            if (currentNode->forward != NULL)
                currentNode = currentNode->forward;
        }

        //moves 1 back
        void move_back()
        {
            if (currentNode->back != NULL)
                currentNode = currentNode->back;
        }

        //converts the linked list to an array of the given type
        T *to_array()
        {
            T *output = new T[size];
            LinkedNode *tempNode = firstNode;
            for (int i = 0; i < size; i++)
            {
                output[i] = tempNode->data;
                tempNode = tempNode->forward;
            }

            delete tempNode;
            return output;
        }

        //deletes the list, keeps everything clean :)
        ~LinkedList()
        {
            if (firstNode != NULL)
            {
                goto_last();
                while (currentNode->back != NULL)
                {
                    currentNode = currentNode->back;
                    delete currentNode->forward;
                }
                delete currentNode;
            }
        }
    };
} // namespace JustusDevTools