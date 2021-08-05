//
// Generated file, do not edit! Created by nedtool 5.6 from veins/modules/application/traci/modified/P2PParking/messages/ResourceReport.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include "ResourceReport_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp

namespace veins {

// forward
template<typename T, typename A>
std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec);

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
inline std::ostream& operator<<(std::ostream& out,const T&) {return out;}

// operator<< for std::vector<T>
template<typename T, typename A>
inline std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec)
{
    out.put('{');
    for(typename std::vector<T,A>::const_iterator it = vec.begin(); it != vec.end(); ++it)
    {
        if (it != vec.begin()) {
            out.put(','); out.put(' ');
        }
        out << *it;
    }
    out.put('}');
    
    char buf[32];
    sprintf(buf, " (size=%u)", (unsigned int)vec.size());
    out.write(buf, strlen(buf));
    return out;
}

Register_Class(ResourceReport)

ResourceReport::ResourceReport(const char *name, short kind) : ::veins::BaseFrame1609_4(name,kind)
{
    atomics_arraysize = 0;
    this->atomics = 0;
    aggregates_arraysize = 0;
    this->aggregates = 0;
}

ResourceReport::ResourceReport(const ResourceReport& other) : ::veins::BaseFrame1609_4(other)
{
    atomics_arraysize = 0;
    this->atomics = 0;
    aggregates_arraysize = 0;
    this->aggregates = 0;
    copy(other);
}

ResourceReport::~ResourceReport()
{
    delete [] this->atomics;
    delete [] this->aggregates;
}

ResourceReport& ResourceReport::operator=(const ResourceReport& other)
{
    if (this==&other) return *this;
    ::veins::BaseFrame1609_4::operator=(other);
    copy(other);
    return *this;
}

void ResourceReport::copy(const ResourceReport& other)
{
    delete [] this->atomics;
    this->atomics = (other.atomics_arraysize==0) ? nullptr : new AtomicInformation[other.atomics_arraysize];
    atomics_arraysize = other.atomics_arraysize;
    for (unsigned int i=0; i<atomics_arraysize; i++)
        this->atomics[i] = other.atomics[i];
    delete [] this->aggregates;
    this->aggregates = (other.aggregates_arraysize==0) ? nullptr : new AggregateInformation[other.aggregates_arraysize];
    aggregates_arraysize = other.aggregates_arraysize;
    for (unsigned int i=0; i<aggregates_arraysize; i++)
        this->aggregates[i] = other.aggregates[i];
}

void ResourceReport::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::veins::BaseFrame1609_4::parsimPack(b);
    b->pack(atomics_arraysize);
    doParsimArrayPacking(b,this->atomics,atomics_arraysize);
    b->pack(aggregates_arraysize);
    doParsimArrayPacking(b,this->aggregates,aggregates_arraysize);
}

void ResourceReport::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::veins::BaseFrame1609_4::parsimUnpack(b);
    delete [] this->atomics;
    b->unpack(atomics_arraysize);
    if (atomics_arraysize==0) {
        this->atomics = 0;
    } else {
        this->atomics = new AtomicInformation[atomics_arraysize];
        doParsimArrayUnpacking(b,this->atomics,atomics_arraysize);
    }
    delete [] this->aggregates;
    b->unpack(aggregates_arraysize);
    if (aggregates_arraysize==0) {
        this->aggregates = 0;
    } else {
        this->aggregates = new AggregateInformation[aggregates_arraysize];
        doParsimArrayUnpacking(b,this->aggregates,aggregates_arraysize);
    }
}

void ResourceReport::setAtomicsArraySize(unsigned int size)
{
    AtomicInformation *atomics2 = (size==0) ? nullptr : new AtomicInformation[size];
    unsigned int sz = atomics_arraysize < size ? atomics_arraysize : size;
    for (unsigned int i=0; i<sz; i++)
        atomics2[i] = this->atomics[i];
    atomics_arraysize = size;
    delete [] this->atomics;
    this->atomics = atomics2;
}

unsigned int ResourceReport::getAtomicsArraySize() const
{
    return atomics_arraysize;
}

AtomicInformation& ResourceReport::getAtomics(unsigned int k)
{
    if (k>=atomics_arraysize) throw omnetpp::cRuntimeError("Array of size %d indexed by %d", atomics_arraysize, k);
    return this->atomics[k];
}

void ResourceReport::setAtomics(unsigned int k, const AtomicInformation& atomics)
{
    if (k>=atomics_arraysize) throw omnetpp::cRuntimeError("Array of size %d indexed by %d", atomics_arraysize, k);
    this->atomics[k] = atomics;
}

void ResourceReport::setAggregatesArraySize(unsigned int size)
{
    AggregateInformation *aggregates2 = (size==0) ? nullptr : new AggregateInformation[size];
    unsigned int sz = aggregates_arraysize < size ? aggregates_arraysize : size;
    for (unsigned int i=0; i<sz; i++)
        aggregates2[i] = this->aggregates[i];
    aggregates_arraysize = size;
    delete [] this->aggregates;
    this->aggregates = aggregates2;
}

unsigned int ResourceReport::getAggregatesArraySize() const
{
    return aggregates_arraysize;
}

AggregateInformation& ResourceReport::getAggregates(unsigned int k)
{
    if (k>=aggregates_arraysize) throw omnetpp::cRuntimeError("Array of size %d indexed by %d", aggregates_arraysize, k);
    return this->aggregates[k];
}

void ResourceReport::setAggregates(unsigned int k, const AggregateInformation& aggregates)
{
    if (k>=aggregates_arraysize) throw omnetpp::cRuntimeError("Array of size %d indexed by %d", aggregates_arraysize, k);
    this->aggregates[k] = aggregates;
}

class ResourceReportDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    ResourceReportDescriptor();
    virtual ~ResourceReportDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual const char *getFieldDynamicTypeString(void *object, int field, int i) const override;
    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(ResourceReportDescriptor)

ResourceReportDescriptor::ResourceReportDescriptor() : omnetpp::cClassDescriptor("veins::ResourceReport", "veins::BaseFrame1609_4")
{
    propertynames = nullptr;
}

ResourceReportDescriptor::~ResourceReportDescriptor()
{
    delete[] propertynames;
}

bool ResourceReportDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<ResourceReport *>(obj)!=nullptr;
}

const char **ResourceReportDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *ResourceReportDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int ResourceReportDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 2+basedesc->getFieldCount() : 2;
}

unsigned int ResourceReportDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISARRAY | FD_ISCOMPOUND,
        FD_ISARRAY | FD_ISCOMPOUND,
    };
    return (field>=0 && field<2) ? fieldTypeFlags[field] : 0;
}

const char *ResourceReportDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "atomics",
        "aggregates",
    };
    return (field>=0 && field<2) ? fieldNames[field] : nullptr;
}

int ResourceReportDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='a' && strcmp(fieldName, "atomics")==0) return base+0;
    if (fieldName[0]=='a' && strcmp(fieldName, "aggregates")==0) return base+1;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *ResourceReportDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "AtomicInformation",
        "AggregateInformation",
    };
    return (field>=0 && field<2) ? fieldTypeStrings[field] : nullptr;
}

const char **ResourceReportDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *ResourceReportDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int ResourceReportDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    ResourceReport *pp = (ResourceReport *)object; (void)pp;
    switch (field) {
        case 0: return pp->getAtomicsArraySize();
        case 1: return pp->getAggregatesArraySize();
        default: return 0;
    }
}

const char *ResourceReportDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    ResourceReport *pp = (ResourceReport *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string ResourceReportDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    ResourceReport *pp = (ResourceReport *)object; (void)pp;
    switch (field) {
        case 0: {std::stringstream out; out << pp->getAtomics(i); return out.str();}
        case 1: {std::stringstream out; out << pp->getAggregates(i); return out.str();}
        default: return "";
    }
}

bool ResourceReportDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    ResourceReport *pp = (ResourceReport *)object; (void)pp;
    switch (field) {
        default: return false;
    }
}

const char *ResourceReportDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case 0: return omnetpp::opp_typename(typeid(AtomicInformation));
        case 1: return omnetpp::opp_typename(typeid(AggregateInformation));
        default: return nullptr;
    };
}

void *ResourceReportDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    ResourceReport *pp = (ResourceReport *)object; (void)pp;
    switch (field) {
        case 0: return (void *)(&pp->getAtomics(i)); break;
        case 1: return (void *)(&pp->getAggregates(i)); break;
        default: return nullptr;
    }
}

} // namespace veins

