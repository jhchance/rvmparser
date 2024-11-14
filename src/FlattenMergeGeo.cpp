#include <cassert>
#include <regex>

#include "Store.h"

// Flatten with regex
// ==================
//
// Simplifies the hierarchy by removing all nodes that doesn't have a name that
// matches a regex. The geometries and attributes of discarded node are moved to
// the neareast ancestor that is kept.

namespace {
  
  struct Context
  {
    Store* store;
    Logger logger;
    std::regex re;
    //std::regex re_debug;
  };

  // check all children which name matches the keywords, 
  // then set group.id to -10000 to sign that the group will not be merged.
 
  void handleGroup(Context& ctx, Node* child)
  {
        
    //leaf groups
    if (child->children.first == nullptr) {
        /*
        if (std::regex_search(child->group.name, ctx.re_debug)) {
            ctx.logger(2, "regular expression search '%s'", child->group.name);
        }
        */
        if (std::regex_search(child->group.name, ctx.re)) {
            child->group.id = -10000;
        }
    }
    else {
        for (Node* subGroup = child->children.first; subGroup != nullptr; subGroup = subGroup->next) {
            handleGroup(ctx, subGroup);
        }
    }
    
  }

}

bool flattenMerge(Store* store, Logger logger, const char* regex)
{
  Context ctx{
    .store = store,
    .logger = logger,
  };

  try {
    ctx.re = std::regex(regex);
    //ctx.re_debug = std::regex("TV-");
  }
  catch (std::regex_error& e) {
    ctx.logger(2, "Failed to compile regular expression '%s': %s", "sdf", e.what());
    return false;
  }

  // The three lowest levels, file, model and first group are always kept
  for (Node* root = store->getFirstRoot(); root; root = root->next) {
    for (Node* model = root->children.first; model; model = model->next) {
      for (Node* group = model->children.first; group; group = group->next) {
          handleGroup(ctx, group);
      }
    }
  }

  return true;
}
