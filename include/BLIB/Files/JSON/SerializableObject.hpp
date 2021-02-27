#ifndef BLIB_FILES_JSON_SERIALIZABLEOBJECT_HPP
#define BLIB_FILES_JSON_SERIALIZABLEOBJECT_HPP

#include <BLIB/Files/JSON/JSON.hpp>
#include <vector>

namespace bl
{
namespace json
{
class SerializableFieldBase;

/**
 * @brief Base class for user defined classes meant to be serialized to and from JSON. Data members
 *        of serializable objects should be of type SerializableField. JSON native types, vector,
 *        unordered_map, and nested SerializableObject members are supported out of the box. For
 *        user defined types as fields they must either implement SerializableObject themselves, or
 *        a specialization of Serializer must be provided for the desired type
 *
 * @ingroup JSON
 *
 */
class SerializableObject {
public:
    /**
     * @brief Deserializes the given json into the fields of this object
     *
     * @param value The json data to populate fields with
     * @return True on success, false on error
     */
    bool deserialize(const Group& value);

    /**
     * @brief Serializes this object and its fields into the returned json data
     *
     * @return Group json data containing this object and its fields
     */
    Group serialize() const;

private:
    std::vector<SerializableFieldBase*> fields;

    void addField(SerializableFieldBase* field);

    friend class SerializableFieldBase;
};

} // namespace json
} // namespace bl

#endif
