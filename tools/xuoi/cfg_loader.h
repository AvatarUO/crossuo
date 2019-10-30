/* clang-format off */
#ifndef CFG_NAME
#error "CFG_NAME not defined"
#define CFG_NAME "error"
#endif

#define CFG_STRINGIFY(X) CFG_STRINGIFY2(X)
#define CFG_STRINGIFY2(X) #X

#define SECTION_FILTER_NAME CFG_STRINGIFY(CFG_NAME)
#define CFG_FILENAME        CFG_STRINGIFY(CFG_NAME) ".cfg"

namespace CFG_NAME {

struct entry
{
    #define CFG_FIELD(s, n, default, t) t s##_##n; std::string raw_##s##_##n;
    #include CFG_DEFINITION
};

entry default_entry()
{
    entry v = {
        #define CFG_FIELD(s, n, default, t) {}, std::string(default),
        #include CFG_DEFINITION
    };

    #define CFG_FIELD(s, n, default, t) convert(v.s##_##n, v.raw_##s##_##n.c_str());
    #include CFG_DEFINITION

    return v;
}

struct data
{
    std::vector<std::string> errors;
    std::vector<entry> entries;
    entry current = default_entry();
    int lineno = 0;
    bool dirty = false;
    bool strict = true;
};

void type_save_current(data &data)
{
    if (!data.dirty)
        return;

    LOG_TRACE("saving entry\n");
    data.entries.emplace_back(data.current);
    data.current = default_entry();
    data.dirty = false;
}

int type_loader(void* user, const char* section, const char* name, const char* value, int lineno)
{
    assert(user && "parser user state not provided");
    auto &obj = *(data *)user;
    obj.lineno = lineno;

#if defined(SECTION_FILTER_NAME)
    if (!name && !value && !strcasecmp(SECTION_FILTER_NAME, section))
    {
        type_save_current(obj);
        LOG_TRACE("new section found\n");
        return 1;
    }
#endif

    auto &entry = obj.current;
    LOG_TRACE("%d: %s.%s = %s\n", lineno, section, name, value);
    if (!name || strlen(name) == 0)
    {
        obj.errors.push_back(std::to_string(lineno) + ": invalid entry");
        return 0;
    }

    bool found = false;
    if (0) ;
#define CFG_FIELD(s, n, default, t) \
    else if (strcasecmp(section, #s) == 0 && strcasecmp(name, #n) == 0) {\
        convert(entry.s##_##n, value); \
        entry.raw_##s##_##n = value; \
        found = true; \
    }
#include CFG_DEFINITION

    if (obj.strict && !found)
    {
        obj.errors.push_back(std::to_string(lineno) + ": unknown entry '" + name + "'");
        return 0;
    }

    obj.dirty = true;
    return 1;
}

void dump(entry *entry)
{
    #define CFG_FIELD(s, n, default, t) LOG_DEBUG("%s_%s = %s\n", #s, #n, entry->raw_##s##_##n.c_str());
    #include CFG_DEFINITION
}

data cfg()
{
    const char *filename = CFG_FILENAME;
    data obj;
    if (ini_parse(filename, type_loader, &obj))
    {
        for (auto &e : obj.errors)
            LOG_ERROR("%s:%s\n", filename, e.c_str());
        LOG_ERROR("failed to parse '%s'\n", filename);
        return {};
    }
    type_save_current(obj);
    return obj;
}

};

#undef TYPE_NAME
#undef SECTION_FILTER_NAME
#undef CFG_DEFINITION
#undef CFG_FILENAME
#undef CFG_STRINGIFY2
#undef CFG_STRINGIFY