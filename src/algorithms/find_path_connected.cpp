#include "find_path_connected.hpp"
#include <unordered_set>
#include <queue>
#include "../handlegraph/types.hpp"
std::unordered_map<const handle_t, vector<handle_t> > find_path_connected(const HandleGraph* g){
    unordered_map<const handle_t, vector<handle_t> > vectors = unordered_map<const handle_t, vector<handle_t> >();
    g->for_each_handle([&](const handle_t& parent_handle){
        unordered_map<const handle_t,bool> visitmap = unordered_map<const handle_t, bool>();
        unordered_set<handle_t> visited_handles = unordered_set<handle_t>();
        queue<const handle_t> queue_left;
        queue<const handle_t> queue_right;
        
        g->follow_edges(parent_handle, true, [&](const handle_t& handle){
            queue_right.push(handle);
            return true;
        });
        g->follow_edges(parent_handle, false, [&](const handle_t& handle){
            queue_right.push(handle);
            return true;
        });
        while(queue_left.size()!=0 && queue_right.size()!=0){
            const handle_t currnode = queue_left.front();
            queue_left.pop();
            g->follow_edges(currnode, true, [&](const handle_t& handle){
                if(visitmap.find(handle)==visitmap.end()){
                    visited_handles.insert(const_cast<handle_t&>(handle));
                    queue_left.push(handle);
                    visitmap[handle] = true;
                }
                return true;
            });
        }
        while(queue_right.size()!=0){
            const handle_t currnode = queue_right.front();
            queue_left.pop();
            g->follow_edges(currnode, false, [&](const handle_t& handle){
                if(visitmap.find(handle)==visitmap.end()){
                    visited_handles.insert(const_cast<handle_t&>(handle));
                    queue_right.push(handle);
                    visitmap[handle] = true;
                }
                return true;
            });
        }
        /*
        queue.push(handle);
        colormap[handle] = 'g';
        while(queue.size()!=0){
            const handle_t currnode = queue.front();
            queue.pop();
            follow_edges_impl(currnode, true, [&](const handle_t& handle){
                if(colormap.find(handle)!=colormap.end() && colormap[handle]=='b'){
                    //push entire reachable list if black
                    //if already discovered by someone else
                    //
                    //return true;
                }
                else{
                    queue.push(handle);
                    component.push_back(&handle);
                    colormap[handle] = 'g';
                    return true;
                } 
            });
            follow_edges_impl(currnode, false, [&](const handle_t& handle){

            });
        }
        colormap[handle] = 'b';
        vector<const handle_t*>::iterator iter= component.begin();
        while(iter!=component.end()){
            reachable_nodes.emplace(get_id(**iter), component);
            ++iter;
        }
        return true;*/
        unordered_set<handle_t>::iterator iter = visited_handles.begin();
        vectors[parent_handle] = vector<handle_t>();
        while(iter!=visited_handles.end()){
            vectors[parent_handle].push_back(*iter);
        }
        return true;

    }, false);
    
    return vectors;

}
void print_path_connected(const HandleGraph *g, unordered_map<const handle_t, vector<handle_t> > map){
    unordered_map<const handle_t, vector<handle_t> >::iterator iter = map.begin();
    vector<handle_t>::iterator veciter;
    handle_t node;
    while(iter!=map.end()){
        node = iter->first;

        cout <<g->get_id(node) << ": ";

        veciter = iter->second.begin();
        while(veciter!=iter->second.end()){
            cout << g->get_id(*veciter) << ", ";
        }
        cout << "\n";
        
    }
}
