//
// Generated file, do not edit! Created by nedtool 5.6 from FeudalMsg.msg.
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
#include "FeudalMsg_m.h"

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

Register_Class(SimpleArrayMsg)

SimpleArrayMsg::SimpleArrayMsg(const char *name, short kind) : ::omnetpp::cPacket(name,kind)
{
    data_arraysize = 0;
    this->data = 0;
}

SimpleArrayMsg::SimpleArrayMsg(const SimpleArrayMsg& other) : ::omnetpp::cPacket(other)
{
    data_arraysize = 0;
    this->data = 0;
    copy(other);
}

SimpleArrayMsg::~SimpleArrayMsg()
{
    delete [] this->data;
}

SimpleArrayMsg& SimpleArrayMsg::operator=(const SimpleArrayMsg& other)
{
    if (this==&other) return *this;
    ::omnetpp::cPacket::operator=(other);
    copy(other);
    return *this;
}

void SimpleArrayMsg::copy(const SimpleArrayMsg& other)
{
    delete [] this->data;
    this->data = (other.data_arraysize==0) ? nullptr : new uint8_t[other.data_arraysize];
    data_arraysize = other.data_arraysize;
    for (unsigned int i=0; i<data_arraysize; i++)
        this->data[i] = other.data[i];
}

void SimpleArrayMsg::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cPacket::parsimPack(b);
    b->pack(data_arraysize);
    doParsimArrayPacking(b,this->data,data_arraysize);
}

void SimpleArrayMsg::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cPacket::parsimUnpack(b);
    delete [] this->data;
    b->unpack(data_arraysize);
    if (data_arraysize==0) {
        this->data = 0;
    } else {
        this->data = new uint8_t[data_arraysize];
        doParsimArrayUnpacking(b,this->data,data_arraysize);
    }
}

void SimpleArrayMsg::setDataArraySize(unsigned int size)
{
    uint8_t *data2 = (size==0) ? nullptr : new uint8_t[size];
    unsigned int sz = data_arraysize < size ? data_arraysize : size;
    for (unsigned int i=0; i<sz; i++)
        data2[i] = this->data[i];
    for (unsigned int i=sz; i<size; i++)
        data2[i] = 0;
    data_arraysize = size;
    delete [] this->data;
    this->data = data2;
}

unsigned int SimpleArrayMsg::getDataArraySize() const
{
    return data_arraysize;
}

uint8_t SimpleArrayMsg::getData(unsigned int k) const
{
    if (k>=data_arraysize) throw omnetpp::cRuntimeError("Array of size %d indexed by %d", data_arraysize, k);
    return this->data[k];
}

void SimpleArrayMsg::setData(unsigned int k, uint8_t data)
{
    if (k>=data_arraysize) throw omnetpp::cRuntimeError("Array of size %d indexed by %d", data_arraysize, k);
    this->data[k] = data;
}

class SimpleArrayMsgDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    SimpleArrayMsgDescriptor();
    virtual ~SimpleArrayMsgDescriptor();

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

Register_ClassDescriptor(SimpleArrayMsgDescriptor)

SimpleArrayMsgDescriptor::SimpleArrayMsgDescriptor() : omnetpp::cClassDescriptor("SimpleArrayMsg", "omnetpp::cPacket")
{
    propertynames = nullptr;
}

SimpleArrayMsgDescriptor::~SimpleArrayMsgDescriptor()
{
    delete[] propertynames;
}

bool SimpleArrayMsgDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<SimpleArrayMsg *>(obj)!=nullptr;
}

const char **SimpleArrayMsgDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *SimpleArrayMsgDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int SimpleArrayMsgDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 1+basedesc->getFieldCount() : 1;
}

unsigned int SimpleArrayMsgDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISARRAY | FD_ISEDITABLE,
    };
    return (field>=0 && field<1) ? fieldTypeFlags[field] : 0;
}

const char *SimpleArrayMsgDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "data",
    };
    return (field>=0 && field<1) ? fieldNames[field] : nullptr;
}

int SimpleArrayMsgDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='d' && strcmp(fieldName, "data")==0) return base+0;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *SimpleArrayMsgDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "uint8_t",
    };
    return (field>=0 && field<1) ? fieldTypeStrings[field] : nullptr;
}

const char **SimpleArrayMsgDescriptor::getFieldPropertyNames(int field) const
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

const char *SimpleArrayMsgDescriptor::getFieldProperty(int field, const char *propertyname) const
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

int SimpleArrayMsgDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    SimpleArrayMsg *pp = (SimpleArrayMsg *)object; (void)pp;
    switch (field) {
        case 0: return pp->getDataArraySize();
        default: return 0;
    }
}

const char *SimpleArrayMsgDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    SimpleArrayMsg *pp = (SimpleArrayMsg *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string SimpleArrayMsgDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    SimpleArrayMsg *pp = (SimpleArrayMsg *)object; (void)pp;
    switch (field) {
        case 0: return ulong2string(pp->getData(i));
        default: return "";
    }
}

bool SimpleArrayMsgDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    SimpleArrayMsg *pp = (SimpleArrayMsg *)object; (void)pp;
    switch (field) {
        case 0: pp->setData(i,string2ulong(value)); return true;
        default: return false;
    }
}

const char *SimpleArrayMsgDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

void *SimpleArrayMsgDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    SimpleArrayMsg *pp = (SimpleArrayMsg *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

Register_Class(VassalMsg)

VassalMsg::VassalMsg(const char *name, short kind) : ::omnetpp::cPacket(name,kind)
{
    this->vassalageID = 0;
    cams_arraysize = 0;
    this->cams = 0;
    denms_arraysize = 0;
    this->denms = 0;
}

VassalMsg::VassalMsg(const VassalMsg& other) : ::omnetpp::cPacket(other)
{
    cams_arraysize = 0;
    this->cams = 0;
    denms_arraysize = 0;
    this->denms = 0;
    copy(other);
}

VassalMsg::~VassalMsg()
{
    for (unsigned int i=0; i<cams_arraysize; i++)
        drop(&(this->cams[i]));
    delete [] this->cams;
    for (unsigned int i=0; i<denms_arraysize; i++)
        drop(&(this->denms[i]));
    delete [] this->denms;
}

VassalMsg& VassalMsg::operator=(const VassalMsg& other)
{
    if (this==&other) return *this;
    ::omnetpp::cPacket::operator=(other);
    copy(other);
    return *this;
}

void VassalMsg::copy(const VassalMsg& other)
{
    this->vassalageID = other.vassalageID;
    delete [] this->cams;
    this->cams = (other.cams_arraysize==0) ? nullptr : new SimpleArrayMsg[other.cams_arraysize];
    cams_arraysize = other.cams_arraysize;
    for (unsigned int i=0; i<cams_arraysize; i++)
    {
        take(&(this->cams[i]));
        this->cams[i] = other.cams[i];
        this->cams[i].setName(other.cams[i].getName());
    }
    delete [] this->denms;
    this->denms = (other.denms_arraysize==0) ? nullptr : new SimpleArrayMsg[other.denms_arraysize];
    denms_arraysize = other.denms_arraysize;
    for (unsigned int i=0; i<denms_arraysize; i++)
    {
        take(&(this->denms[i]));
        this->denms[i] = other.denms[i];
        this->denms[i].setName(other.denms[i].getName());
    }
}

void VassalMsg::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cPacket::parsimPack(b);
    doParsimPacking(b,this->vassalageID);
    b->pack(cams_arraysize);
    doParsimArrayPacking(b,this->cams,cams_arraysize);
    b->pack(denms_arraysize);
    doParsimArrayPacking(b,this->denms,denms_arraysize);
}

void VassalMsg::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cPacket::parsimUnpack(b);
    doParsimUnpacking(b,this->vassalageID);
    delete [] this->cams;
    b->unpack(cams_arraysize);
    if (cams_arraysize==0) {
        this->cams = 0;
    } else {
        this->cams = new SimpleArrayMsg[cams_arraysize];
        doParsimArrayUnpacking(b,this->cams,cams_arraysize);
    }
    delete [] this->denms;
    b->unpack(denms_arraysize);
    if (denms_arraysize==0) {
        this->denms = 0;
    } else {
        this->denms = new SimpleArrayMsg[denms_arraysize];
        doParsimArrayUnpacking(b,this->denms,denms_arraysize);
    }
}

uint32_t VassalMsg::getVassalageID() const
{
    return this->vassalageID;
}

void VassalMsg::setVassalageID(uint32_t vassalageID)
{
    this->vassalageID = vassalageID;
}

void VassalMsg::setCamsArraySize(unsigned int size)
{
    SimpleArrayMsg *cams2 = (size==0) ? nullptr : new SimpleArrayMsg[size];
    unsigned int sz = cams_arraysize < size ? cams_arraysize : size;
    for (unsigned int i=0; i<sz; i++)
        cams2[i] = this->cams[i];
    for (unsigned int i=sz; i<size; i++)
        take(&(cams2[i]));
    cams_arraysize = size;
    delete [] this->cams;
    this->cams = cams2;
}

unsigned int VassalMsg::getCamsArraySize() const
{
    return cams_arraysize;
}

SimpleArrayMsg& VassalMsg::getCams(unsigned int k)
{
    if (k>=cams_arraysize) throw omnetpp::cRuntimeError("Array of size %d indexed by %d", cams_arraysize, k);
    return this->cams[k];
}

void VassalMsg::setCams(unsigned int k, const SimpleArrayMsg& cams)
{
    if (k>=cams_arraysize) throw omnetpp::cRuntimeError("Array of size %d indexed by %d", cams_arraysize, k);
    this->cams[k] = cams;
}

void VassalMsg::setDenmsArraySize(unsigned int size)
{
    SimpleArrayMsg *denms2 = (size==0) ? nullptr : new SimpleArrayMsg[size];
    unsigned int sz = denms_arraysize < size ? denms_arraysize : size;
    for (unsigned int i=0; i<sz; i++)
        denms2[i] = this->denms[i];
    for (unsigned int i=sz; i<size; i++)
        take(&(denms2[i]));
    denms_arraysize = size;
    delete [] this->denms;
    this->denms = denms2;
}

unsigned int VassalMsg::getDenmsArraySize() const
{
    return denms_arraysize;
}

SimpleArrayMsg& VassalMsg::getDenms(unsigned int k)
{
    if (k>=denms_arraysize) throw omnetpp::cRuntimeError("Array of size %d indexed by %d", denms_arraysize, k);
    return this->denms[k];
}

void VassalMsg::setDenms(unsigned int k, const SimpleArrayMsg& denms)
{
    if (k>=denms_arraysize) throw omnetpp::cRuntimeError("Array of size %d indexed by %d", denms_arraysize, k);
    this->denms[k] = denms;
}

class VassalMsgDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    VassalMsgDescriptor();
    virtual ~VassalMsgDescriptor();

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

Register_ClassDescriptor(VassalMsgDescriptor)

VassalMsgDescriptor::VassalMsgDescriptor() : omnetpp::cClassDescriptor("VassalMsg", "omnetpp::cPacket")
{
    propertynames = nullptr;
}

VassalMsgDescriptor::~VassalMsgDescriptor()
{
    delete[] propertynames;
}

bool VassalMsgDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<VassalMsg *>(obj)!=nullptr;
}

const char **VassalMsgDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *VassalMsgDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int VassalMsgDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 3+basedesc->getFieldCount() : 3;
}

unsigned int VassalMsgDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISARRAY | FD_ISCOMPOUND | FD_ISCOBJECT | FD_ISCOWNEDOBJECT,
        FD_ISARRAY | FD_ISCOMPOUND | FD_ISCOBJECT | FD_ISCOWNEDOBJECT,
    };
    return (field>=0 && field<3) ? fieldTypeFlags[field] : 0;
}

const char *VassalMsgDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "vassalageID",
        "cams",
        "denms",
    };
    return (field>=0 && field<3) ? fieldNames[field] : nullptr;
}

int VassalMsgDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='v' && strcmp(fieldName, "vassalageID")==0) return base+0;
    if (fieldName[0]=='c' && strcmp(fieldName, "cams")==0) return base+1;
    if (fieldName[0]=='d' && strcmp(fieldName, "denms")==0) return base+2;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *VassalMsgDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "uint32_t",
        "SimpleArrayMsg",
        "SimpleArrayMsg",
    };
    return (field>=0 && field<3) ? fieldTypeStrings[field] : nullptr;
}

const char **VassalMsgDescriptor::getFieldPropertyNames(int field) const
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

const char *VassalMsgDescriptor::getFieldProperty(int field, const char *propertyname) const
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

int VassalMsgDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    VassalMsg *pp = (VassalMsg *)object; (void)pp;
    switch (field) {
        case 1: return pp->getCamsArraySize();
        case 2: return pp->getDenmsArraySize();
        default: return 0;
    }
}

const char *VassalMsgDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    VassalMsg *pp = (VassalMsg *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string VassalMsgDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    VassalMsg *pp = (VassalMsg *)object; (void)pp;
    switch (field) {
        case 0: return ulong2string(pp->getVassalageID());
        case 1: {std::stringstream out; out << pp->getCams(i); return out.str();}
        case 2: {std::stringstream out; out << pp->getDenms(i); return out.str();}
        default: return "";
    }
}

bool VassalMsgDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    VassalMsg *pp = (VassalMsg *)object; (void)pp;
    switch (field) {
        case 0: pp->setVassalageID(string2ulong(value)); return true;
        default: return false;
    }
}

const char *VassalMsgDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case 1: return omnetpp::opp_typename(typeid(SimpleArrayMsg));
        case 2: return omnetpp::opp_typename(typeid(SimpleArrayMsg));
        default: return nullptr;
    };
}

void *VassalMsgDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    VassalMsg *pp = (VassalMsg *)object; (void)pp;
    switch (field) {
        case 1: return (void *)static_cast<omnetpp::cObject *>(&pp->getCams(i)); break;
        case 2: return (void *)static_cast<omnetpp::cObject *>(&pp->getDenms(i)); break;
        default: return nullptr;
    }
}

Register_Class(FeudalSync)

FeudalSync::FeudalSync(const char *name, short kind) : ::omnetpp::cPacket(name,kind)
{
    this->ref = 0;
    members_arraysize = 0;
    this->members = 0;
    lteAddresses_arraysize = 0;
    this->lteAddresses = 0;
}

FeudalSync::FeudalSync(const FeudalSync& other) : ::omnetpp::cPacket(other)
{
    members_arraysize = 0;
    this->members = 0;
    lteAddresses_arraysize = 0;
    this->lteAddresses = 0;
    copy(other);
}

FeudalSync::~FeudalSync()
{
    delete [] this->members;
    delete [] this->lteAddresses;
}

FeudalSync& FeudalSync::operator=(const FeudalSync& other)
{
    if (this==&other) return *this;
    ::omnetpp::cPacket::operator=(other);
    copy(other);
    return *this;
}

void FeudalSync::copy(const FeudalSync& other)
{
    this->ref = other.ref;
    delete [] this->members;
    this->members = (other.members_arraysize==0) ? nullptr : new uint32_t[other.members_arraysize];
    members_arraysize = other.members_arraysize;
    for (unsigned int i=0; i<members_arraysize; i++)
        this->members[i] = other.members[i];
    delete [] this->lteAddresses;
    this->lteAddresses = (other.lteAddresses_arraysize==0) ? nullptr : new uint32_t[other.lteAddresses_arraysize];
    lteAddresses_arraysize = other.lteAddresses_arraysize;
    for (unsigned int i=0; i<lteAddresses_arraysize; i++)
        this->lteAddresses[i] = other.lteAddresses[i];
}

void FeudalSync::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cPacket::parsimPack(b);
    doParsimPacking(b,this->ref);
    b->pack(members_arraysize);
    doParsimArrayPacking(b,this->members,members_arraysize);
    b->pack(lteAddresses_arraysize);
    doParsimArrayPacking(b,this->lteAddresses,lteAddresses_arraysize);
}

void FeudalSync::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cPacket::parsimUnpack(b);
    doParsimUnpacking(b,this->ref);
    delete [] this->members;
    b->unpack(members_arraysize);
    if (members_arraysize==0) {
        this->members = 0;
    } else {
        this->members = new uint32_t[members_arraysize];
        doParsimArrayUnpacking(b,this->members,members_arraysize);
    }
    delete [] this->lteAddresses;
    b->unpack(lteAddresses_arraysize);
    if (lteAddresses_arraysize==0) {
        this->lteAddresses = 0;
    } else {
        this->lteAddresses = new uint32_t[lteAddresses_arraysize];
        doParsimArrayUnpacking(b,this->lteAddresses,lteAddresses_arraysize);
    }
}

uint16_t FeudalSync::getRef() const
{
    return this->ref;
}

void FeudalSync::setRef(uint16_t ref)
{
    this->ref = ref;
}

void FeudalSync::setMembersArraySize(unsigned int size)
{
    uint32_t *members2 = (size==0) ? nullptr : new uint32_t[size];
    unsigned int sz = members_arraysize < size ? members_arraysize : size;
    for (unsigned int i=0; i<sz; i++)
        members2[i] = this->members[i];
    for (unsigned int i=sz; i<size; i++)
        members2[i] = 0;
    members_arraysize = size;
    delete [] this->members;
    this->members = members2;
}

unsigned int FeudalSync::getMembersArraySize() const
{
    return members_arraysize;
}

uint32_t FeudalSync::getMembers(unsigned int k) const
{
    if (k>=members_arraysize) throw omnetpp::cRuntimeError("Array of size %d indexed by %d", members_arraysize, k);
    return this->members[k];
}

void FeudalSync::setMembers(unsigned int k, uint32_t members)
{
    if (k>=members_arraysize) throw omnetpp::cRuntimeError("Array of size %d indexed by %d", members_arraysize, k);
    this->members[k] = members;
}

void FeudalSync::setLteAddressesArraySize(unsigned int size)
{
    uint32_t *lteAddresses2 = (size==0) ? nullptr : new uint32_t[size];
    unsigned int sz = lteAddresses_arraysize < size ? lteAddresses_arraysize : size;
    for (unsigned int i=0; i<sz; i++)
        lteAddresses2[i] = this->lteAddresses[i];
    for (unsigned int i=sz; i<size; i++)
        lteAddresses2[i] = 0;
    lteAddresses_arraysize = size;
    delete [] this->lteAddresses;
    this->lteAddresses = lteAddresses2;
}

unsigned int FeudalSync::getLteAddressesArraySize() const
{
    return lteAddresses_arraysize;
}

uint32_t FeudalSync::getLteAddresses(unsigned int k) const
{
    if (k>=lteAddresses_arraysize) throw omnetpp::cRuntimeError("Array of size %d indexed by %d", lteAddresses_arraysize, k);
    return this->lteAddresses[k];
}

void FeudalSync::setLteAddresses(unsigned int k, uint32_t lteAddresses)
{
    if (k>=lteAddresses_arraysize) throw omnetpp::cRuntimeError("Array of size %d indexed by %d", lteAddresses_arraysize, k);
    this->lteAddresses[k] = lteAddresses;
}

class FeudalSyncDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    FeudalSyncDescriptor();
    virtual ~FeudalSyncDescriptor();

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

Register_ClassDescriptor(FeudalSyncDescriptor)

FeudalSyncDescriptor::FeudalSyncDescriptor() : omnetpp::cClassDescriptor("FeudalSync", "omnetpp::cPacket")
{
    propertynames = nullptr;
}

FeudalSyncDescriptor::~FeudalSyncDescriptor()
{
    delete[] propertynames;
}

bool FeudalSyncDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<FeudalSync *>(obj)!=nullptr;
}

const char **FeudalSyncDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *FeudalSyncDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int FeudalSyncDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 3+basedesc->getFieldCount() : 3;
}

unsigned int FeudalSyncDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISARRAY | FD_ISEDITABLE,
        FD_ISARRAY | FD_ISEDITABLE,
    };
    return (field>=0 && field<3) ? fieldTypeFlags[field] : 0;
}

const char *FeudalSyncDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "ref",
        "members",
        "lteAddresses",
    };
    return (field>=0 && field<3) ? fieldNames[field] : nullptr;
}

int FeudalSyncDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='r' && strcmp(fieldName, "ref")==0) return base+0;
    if (fieldName[0]=='m' && strcmp(fieldName, "members")==0) return base+1;
    if (fieldName[0]=='l' && strcmp(fieldName, "lteAddresses")==0) return base+2;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *FeudalSyncDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "uint16_t",
        "uint32_t",
        "uint32_t",
    };
    return (field>=0 && field<3) ? fieldTypeStrings[field] : nullptr;
}

const char **FeudalSyncDescriptor::getFieldPropertyNames(int field) const
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

const char *FeudalSyncDescriptor::getFieldProperty(int field, const char *propertyname) const
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

int FeudalSyncDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    FeudalSync *pp = (FeudalSync *)object; (void)pp;
    switch (field) {
        case 1: return pp->getMembersArraySize();
        case 2: return pp->getLteAddressesArraySize();
        default: return 0;
    }
}

const char *FeudalSyncDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    FeudalSync *pp = (FeudalSync *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string FeudalSyncDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    FeudalSync *pp = (FeudalSync *)object; (void)pp;
    switch (field) {
        case 0: return ulong2string(pp->getRef());
        case 1: return ulong2string(pp->getMembers(i));
        case 2: return ulong2string(pp->getLteAddresses(i));
        default: return "";
    }
}

bool FeudalSyncDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    FeudalSync *pp = (FeudalSync *)object; (void)pp;
    switch (field) {
        case 0: pp->setRef(string2ulong(value)); return true;
        case 1: pp->setMembers(i,string2ulong(value)); return true;
        case 2: pp->setLteAddresses(i,string2ulong(value)); return true;
        default: return false;
    }
}

const char *FeudalSyncDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

void *FeudalSyncDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    FeudalSync *pp = (FeudalSync *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

Register_Class(FeudalMsg)

FeudalMsg::FeudalMsg(const char *name, short kind) : ::omnetpp::cPacket(name,kind)
{
    this->msgType = 0;
    this->txStationID = 0;
    this->timestamp = 0;
    this->platoonID = 0;
    this->vassalageID = 0;
    syncMsg_arraysize = 0;
    this->syncMsg = 0;
    vassalMsg_arraysize = 0;
    this->vassalMsg = 0;
}

FeudalMsg::FeudalMsg(const FeudalMsg& other) : ::omnetpp::cPacket(other)
{
    syncMsg_arraysize = 0;
    this->syncMsg = 0;
    vassalMsg_arraysize = 0;
    this->vassalMsg = 0;
    copy(other);
}

FeudalMsg::~FeudalMsg()
{
    for (unsigned int i=0; i<syncMsg_arraysize; i++)
        drop(&(this->syncMsg[i]));
    delete [] this->syncMsg;
    for (unsigned int i=0; i<vassalMsg_arraysize; i++)
        drop(&(this->vassalMsg[i]));
    delete [] this->vassalMsg;
}

FeudalMsg& FeudalMsg::operator=(const FeudalMsg& other)
{
    if (this==&other) return *this;
    ::omnetpp::cPacket::operator=(other);
    copy(other);
    return *this;
}

void FeudalMsg::copy(const FeudalMsg& other)
{
    this->msgType = other.msgType;
    this->txStationID = other.txStationID;
    this->timestamp = other.timestamp;
    this->platoonID = other.platoonID;
    this->vassalageID = other.vassalageID;
    delete [] this->syncMsg;
    this->syncMsg = (other.syncMsg_arraysize==0) ? nullptr : new FeudalSync[other.syncMsg_arraysize];
    syncMsg_arraysize = other.syncMsg_arraysize;
    for (unsigned int i=0; i<syncMsg_arraysize; i++)
    {
        take(&(this->syncMsg[i]));
        this->syncMsg[i] = other.syncMsg[i];
        this->syncMsg[i].setName(other.syncMsg[i].getName());
    }
    delete [] this->vassalMsg;
    this->vassalMsg = (other.vassalMsg_arraysize==0) ? nullptr : new VassalMsg[other.vassalMsg_arraysize];
    vassalMsg_arraysize = other.vassalMsg_arraysize;
    for (unsigned int i=0; i<vassalMsg_arraysize; i++)
    {
        take(&(this->vassalMsg[i]));
        this->vassalMsg[i] = other.vassalMsg[i];
        this->vassalMsg[i].setName(other.vassalMsg[i].getName());
    }
}

void FeudalMsg::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cPacket::parsimPack(b);
    doParsimPacking(b,this->msgType);
    doParsimPacking(b,this->txStationID);
    doParsimPacking(b,this->timestamp);
    doParsimPacking(b,this->platoonID);
    doParsimPacking(b,this->vassalageID);
    b->pack(syncMsg_arraysize);
    doParsimArrayPacking(b,this->syncMsg,syncMsg_arraysize);
    b->pack(vassalMsg_arraysize);
    doParsimArrayPacking(b,this->vassalMsg,vassalMsg_arraysize);
}

void FeudalMsg::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cPacket::parsimUnpack(b);
    doParsimUnpacking(b,this->msgType);
    doParsimUnpacking(b,this->txStationID);
    doParsimUnpacking(b,this->timestamp);
    doParsimUnpacking(b,this->platoonID);
    doParsimUnpacking(b,this->vassalageID);
    delete [] this->syncMsg;
    b->unpack(syncMsg_arraysize);
    if (syncMsg_arraysize==0) {
        this->syncMsg = 0;
    } else {
        this->syncMsg = new FeudalSync[syncMsg_arraysize];
        doParsimArrayUnpacking(b,this->syncMsg,syncMsg_arraysize);
    }
    delete [] this->vassalMsg;
    b->unpack(vassalMsg_arraysize);
    if (vassalMsg_arraysize==0) {
        this->vassalMsg = 0;
    } else {
        this->vassalMsg = new VassalMsg[vassalMsg_arraysize];
        doParsimArrayUnpacking(b,this->vassalMsg,vassalMsg_arraysize);
    }
}

uint8_t FeudalMsg::getMsgType() const
{
    return this->msgType;
}

void FeudalMsg::setMsgType(uint8_t msgType)
{
    this->msgType = msgType;
}

uint32_t FeudalMsg::getTxStationID() const
{
    return this->txStationID;
}

void FeudalMsg::setTxStationID(uint32_t txStationID)
{
    this->txStationID = txStationID;
}

::omnetpp::simtime_t FeudalMsg::getTimestamp() const
{
    return this->timestamp;
}

void FeudalMsg::setTimestamp(::omnetpp::simtime_t timestamp)
{
    this->timestamp = timestamp;
}

uint32_t FeudalMsg::getPlatoonID() const
{
    return this->platoonID;
}

void FeudalMsg::setPlatoonID(uint32_t platoonID)
{
    this->platoonID = platoonID;
}

uint32_t FeudalMsg::getVassalageID() const
{
    return this->vassalageID;
}

void FeudalMsg::setVassalageID(uint32_t vassalageID)
{
    this->vassalageID = vassalageID;
}

void FeudalMsg::setSyncMsgArraySize(unsigned int size)
{
    FeudalSync *syncMsg2 = (size==0) ? nullptr : new FeudalSync[size];
    unsigned int sz = syncMsg_arraysize < size ? syncMsg_arraysize : size;
    for (unsigned int i=0; i<sz; i++)
        syncMsg2[i] = this->syncMsg[i];
    for (unsigned int i=sz; i<size; i++)
        take(&(syncMsg2[i]));
    syncMsg_arraysize = size;
    delete [] this->syncMsg;
    this->syncMsg = syncMsg2;
}

unsigned int FeudalMsg::getSyncMsgArraySize() const
{
    return syncMsg_arraysize;
}

FeudalSync& FeudalMsg::getSyncMsg(unsigned int k)
{
    if (k>=syncMsg_arraysize) throw omnetpp::cRuntimeError("Array of size %d indexed by %d", syncMsg_arraysize, k);
    return this->syncMsg[k];
}

void FeudalMsg::setSyncMsg(unsigned int k, const FeudalSync& syncMsg)
{
    if (k>=syncMsg_arraysize) throw omnetpp::cRuntimeError("Array of size %d indexed by %d", syncMsg_arraysize, k);
    this->syncMsg[k] = syncMsg;
}

void FeudalMsg::setVassalMsgArraySize(unsigned int size)
{
    VassalMsg *vassalMsg2 = (size==0) ? nullptr : new VassalMsg[size];
    unsigned int sz = vassalMsg_arraysize < size ? vassalMsg_arraysize : size;
    for (unsigned int i=0; i<sz; i++)
        vassalMsg2[i] = this->vassalMsg[i];
    for (unsigned int i=sz; i<size; i++)
        take(&(vassalMsg2[i]));
    vassalMsg_arraysize = size;
    delete [] this->vassalMsg;
    this->vassalMsg = vassalMsg2;
}

unsigned int FeudalMsg::getVassalMsgArraySize() const
{
    return vassalMsg_arraysize;
}

VassalMsg& FeudalMsg::getVassalMsg(unsigned int k)
{
    if (k>=vassalMsg_arraysize) throw omnetpp::cRuntimeError("Array of size %d indexed by %d", vassalMsg_arraysize, k);
    return this->vassalMsg[k];
}

void FeudalMsg::setVassalMsg(unsigned int k, const VassalMsg& vassalMsg)
{
    if (k>=vassalMsg_arraysize) throw omnetpp::cRuntimeError("Array of size %d indexed by %d", vassalMsg_arraysize, k);
    this->vassalMsg[k] = vassalMsg;
}

class FeudalMsgDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    FeudalMsgDescriptor();
    virtual ~FeudalMsgDescriptor();

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

Register_ClassDescriptor(FeudalMsgDescriptor)

FeudalMsgDescriptor::FeudalMsgDescriptor() : omnetpp::cClassDescriptor("FeudalMsg", "omnetpp::cPacket")
{
    propertynames = nullptr;
}

FeudalMsgDescriptor::~FeudalMsgDescriptor()
{
    delete[] propertynames;
}

bool FeudalMsgDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<FeudalMsg *>(obj)!=nullptr;
}

const char **FeudalMsgDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *FeudalMsgDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int FeudalMsgDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 7+basedesc->getFieldCount() : 7;
}

unsigned int FeudalMsgDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISARRAY | FD_ISCOMPOUND | FD_ISCOBJECT | FD_ISCOWNEDOBJECT,
        FD_ISARRAY | FD_ISCOMPOUND | FD_ISCOBJECT | FD_ISCOWNEDOBJECT,
    };
    return (field>=0 && field<7) ? fieldTypeFlags[field] : 0;
}

const char *FeudalMsgDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "msgType",
        "txStationID",
        "timestamp",
        "platoonID",
        "vassalageID",
        "syncMsg",
        "vassalMsg",
    };
    return (field>=0 && field<7) ? fieldNames[field] : nullptr;
}

int FeudalMsgDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='m' && strcmp(fieldName, "msgType")==0) return base+0;
    if (fieldName[0]=='t' && strcmp(fieldName, "txStationID")==0) return base+1;
    if (fieldName[0]=='t' && strcmp(fieldName, "timestamp")==0) return base+2;
    if (fieldName[0]=='p' && strcmp(fieldName, "platoonID")==0) return base+3;
    if (fieldName[0]=='v' && strcmp(fieldName, "vassalageID")==0) return base+4;
    if (fieldName[0]=='s' && strcmp(fieldName, "syncMsg")==0) return base+5;
    if (fieldName[0]=='v' && strcmp(fieldName, "vassalMsg")==0) return base+6;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *FeudalMsgDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "uint8_t",
        "uint32_t",
        "simtime_t",
        "uint32_t",
        "uint32_t",
        "FeudalSync",
        "VassalMsg",
    };
    return (field>=0 && field<7) ? fieldTypeStrings[field] : nullptr;
}

const char **FeudalMsgDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case 0: {
            static const char *names[] = { "enum",  nullptr };
            return names;
        }
        default: return nullptr;
    }
}

const char *FeudalMsgDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case 0:
            if (!strcmp(propertyname,"enum")) return "MessageTypeFeudalisticPlatoon";
            return nullptr;
        default: return nullptr;
    }
}

int FeudalMsgDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    FeudalMsg *pp = (FeudalMsg *)object; (void)pp;
    switch (field) {
        case 5: return pp->getSyncMsgArraySize();
        case 6: return pp->getVassalMsgArraySize();
        default: return 0;
    }
}

const char *FeudalMsgDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    FeudalMsg *pp = (FeudalMsg *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string FeudalMsgDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    FeudalMsg *pp = (FeudalMsg *)object; (void)pp;
    switch (field) {
        case 0: return enum2string(pp->getMsgType(), "MessageTypeFeudalisticPlatoon");
        case 1: return ulong2string(pp->getTxStationID());
        case 2: return simtime2string(pp->getTimestamp());
        case 3: return ulong2string(pp->getPlatoonID());
        case 4: return ulong2string(pp->getVassalageID());
        case 5: {std::stringstream out; out << pp->getSyncMsg(i); return out.str();}
        case 6: {std::stringstream out; out << pp->getVassalMsg(i); return out.str();}
        default: return "";
    }
}

bool FeudalMsgDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    FeudalMsg *pp = (FeudalMsg *)object; (void)pp;
    switch (field) {
        case 0: pp->setMsgType((MessageTypeFeudalisticPlatoon)string2enum(value, "MessageTypeFeudalisticPlatoon")); return true;
        case 1: pp->setTxStationID(string2ulong(value)); return true;
        case 2: pp->setTimestamp(string2simtime(value)); return true;
        case 3: pp->setPlatoonID(string2ulong(value)); return true;
        case 4: pp->setVassalageID(string2ulong(value)); return true;
        default: return false;
    }
}

const char *FeudalMsgDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case 5: return omnetpp::opp_typename(typeid(FeudalSync));
        case 6: return omnetpp::opp_typename(typeid(VassalMsg));
        default: return nullptr;
    };
}

void *FeudalMsgDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    FeudalMsg *pp = (FeudalMsg *)object; (void)pp;
    switch (field) {
        case 5: return (void *)static_cast<omnetpp::cObject *>(&pp->getSyncMsg(i)); break;
        case 6: return (void *)static_cast<omnetpp::cObject *>(&pp->getVassalMsg(i)); break;
        default: return nullptr;
    }
}


