#include "Common.h"
#include "Store.h"

namespace {

  struct Context
  {
    Map discardTags;
    Store* store;
    Logger logger;
    uint32_t discarded = 0;
    Map mergeTags;
  };

  void readTagList(Context* context, const void* ptr, size_t size)
  {
    auto * a = (const char*)ptr;
    auto * b = a + size;

    uint32_t N = 0;
    while (true) {
      while (a < b && (*a == '\n' || *a == '\r')) a++;
      auto * c = a;
      while (a < b && (*a != '\n' && *a != '\r')) a++;

      if (c < a) {
        auto * d = a - 1;
        while (c < d && (d[-1] != '\t')) --d;

        auto * str = context->store->strings.intern(d, a);
        context->discardTags.insert(uint64_t(str), uint64_t(1 + N++));
      }
      else {
        break;
      }
    }
    context->logger(0, "DiscardGroups: Read %d tags.", N);
  }

  void readMergeList(Context* context, const void* ptr, size_t size) {
      auto* a = (const char*)ptr;
      auto* b = a + size;

      uint32_t N = 0;
      while (true) {
          while (a < b && (*a == '\n' || *a == '\r')) a++;
          auto* c = a;
          while (a < b && (*a != '\n' && *a != '\r')) a++;

          if (c < a) {
              auto* d = a - 1;
              while (c < d && (d[-1] != '\t')) --d;

              auto* str = context->store->strings.intern(d, a);
              context->mergeTags.insert( uint64_t(1 + N++), uint64_t(str));
          } else {
              break;
          }
      }
      context->logger(0, "DiscardGroups: Read %d tags.", N);
  }

  void pruneChildren(Context* context, Node* group)
  {

    ListHeader<Node> groupsNew;
    groupsNew.clear();
    for (auto * child = group->children.first; child; ) {
      auto * next = child->next;
      if (context->discardTags.get(uint64_t(child->group.name))) {
        //context->logger(0, "Discarded %s", child->group.name);
        context->discarded++;
      }
      else {
        pruneChildren(context, child);
        groupsNew.insert(child);
      }
      child = next;
    }
    group->children = groupsNew;
  }

}


bool discardGroups(Store* store, Logger logger, const void* ptr, size_t size)
{
  Context context;
  context.store = store;
  context.logger = logger;
  readTagList(&context, ptr, size);

  for (auto * root = store->getFirstRoot(); root != nullptr; root = root->next) {
    for (auto * model = root->children.first; model != nullptr; model = model->next) {
      pruneChildren(&context, model);
    }
  }
  context.logger(0, "DiscardGroups: Discarded %d groups.", context.discarded);

  return true;
}

//--cjh, ��ȡ���ϲ������������ѧ��discardGroups()
bool mergeGroups(Store* store, Logger logger, const void* ptr, size_t size,std::string& strRegex) 
{
    Context context;
    context.store = store;
    context.logger = logger;
    context.mergeTags.clear();
    readMergeList(&context, ptr, size);

    for (uint64_t i = 0; i < context.mergeTags.fill; i++)
    {
        char* value = (char*) context.mergeTags.get(i + 1);
        if (i == 0) {
            strRegex = value;
        }
        else {
            strRegex = strRegex + "|" + value;
        }
    }

    return true;
}
