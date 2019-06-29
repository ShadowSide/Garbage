using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Xml;
using System.Xml.Serialization;

namespace Global.BusinessCommon.Helpers.Serialization
{
    public static class SerializationHelpers
    {
        /// <summary>
        /// Сериализовать в Json объект из source
        /// </summary>
        /// <param name="dropNullFields">Если true, то null поля игнорируются и не записываются в конечный Json, иначе записываются все поля</param>
        /// <returns></returns>
        public static string JsonSerialize<Source>(Source source, bool dropNullFields = true, JsonConverter[] jsonConverters = null) where Source : class
        {
            if (source is null)
                return null;

            var jsonConverters_ = jsonConverters ?? new JsonConverter[] { };

            if (!dropNullFields)
                return JsonConvert.SerializeObject(source, jsonConverters_);
            return JsonConvert.SerializeObject(source,
                            Newtonsoft.Json.Formatting.None,
                            new JsonSerializerSettings
                            {
                                NullValueHandling = NullValueHandling.Ignore,
                                Converters = jsonConverters_
                            });
        }

        /// <summary>
        /// Десериализовать в объект из Json
        /// </summary>
        /// <typeparam name="Destination">Тип десериализуемого объекта</typeparam>
        /// <param name="source">Строка с Json</param>
        /// <returns>Десериализованный объект</returns>
        public static Destination JsonDeserialize<Destination>(string source, JsonConverter[] jsonConverters = null) where Destination : class
        {
            if (source is null)
                return default(Destination);
            return JsonConvert.DeserializeObject<Destination>(source, jsonConverters ?? new JsonConverter [] { });
        }

        public static object JsonDeserialize(string source, Type type, JsonConverter[] jsonConverters = null) 
        {
            return JsonConvert.DeserializeObject(source, type, jsonConverters ?? new JsonConverter[] { });
        }

        /// <summary>
        /// Сериализовать в XML объект из source
        /// </summary>
        public static string XmlSerialize<Source>(Source source, XmlSerializerNamespaces namespaces = null) where Source : class
        {
            if (source is null)
                return default(string);

            var serializer = new XmlSerializer(typeof(Source));
            var result = "";

            using (var stringWriter = new StringWriter())
            {
                using (XmlWriter xmlWriter = XmlWriter.Create(stringWriter))
                {
                    serializer.Serialize(xmlWriter, source, namespaces ?? EmptyNamespaces);
                    xmlWriter.Flush();
                    stringWriter.Flush();
                    result = stringWriter.ToString();
                }
            }
            return result;
        }

        /// <summary>
        /// Десериализовать в объект из xmlSource в формате XML
        /// </summary>
        public static Destination XmlDeserialize<Destination>(string xmlSource) where Destination : class
        {
            if (xmlSource is null)
                return default(Destination);
            var serializer = new XmlSerializer(typeof(Destination));
            using (var stringReader = new StringReader(xmlSource))
            {
                return (Destination)serializer.Deserialize(stringReader);
            }
        }

        /// <summary>
        /// Сериализовать в XML объект из source в xmlWriter
        /// </summary>
        public static void XmlSerializeTo<Source>(Source source, XmlWriter xmlWriter, XmlSerializerNamespaces namespaces = null) where Source : class
        {
            if (source is null)
                return;

            var serializer = new XmlSerializer(typeof(Source));
            serializer.Serialize(xmlWriter, source, namespaces ?? EmptyNamespaces);
            //xmlWriter.Flush();
        }

        /// <summary>
        /// Десериализовать в объект из xmlReader
        /// </summary>
        public static Destination XmlDeserializeFrom<Destination>(XmlReader xmlReader) where Destination : class
        {
            if (xmlReader is null)
                throw new ArgumentNullException(nameof(xmlReader));
            var serializer = new XmlSerializer(typeof(Destination));
            return (Destination)serializer.Deserialize(xmlReader);
        }

        public static readonly XmlSerializerNamespaces EmptyNamespaces = new XmlSerializerNamespaces(new[] { XmlQualifiedName.Empty });
    }
}
