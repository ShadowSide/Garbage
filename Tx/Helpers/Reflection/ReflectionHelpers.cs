using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Reflection;

namespace Global.BusinessCommon.Helpers.Reflection
{
    public static class ReflectionHelpers
    {
        /// <summary>
        /// Копирует все property с одинаковыми именами и с присваивающимися типами из source в destination
        /// </summary>
        public static void CopyAllPublicPropertiesWithEqualNameAndAssignableTypes<Source, Destination>(Source source, Destination destination)
            where Source: class
            where Destination : class
        {
            if (source is null) throw new ArgumentNullException(nameof(source));
            if (destination is null) throw new ArgumentNullException(nameof(destination));
            //get the list of all properties in the destination object
            var destinationProperties = destination.GetType().GetProperties(BindingFlags.Instance | BindingFlags.Public);

            //get the list of all properties in the source object
            foreach (var sourceProperty in source.GetType().GetProperties(BindingFlags.Instance | BindingFlags.Public))
            {
                foreach (var destinationProperty in destinationProperties)
                {
                    //if we find match between source & destination properties name, set
                    //the value to the destination property
                    if (destinationProperty.Name == sourceProperty.Name 
                        && destinationProperty.PropertyType.IsAssignableFrom(sourceProperty.PropertyType) 
                        && IsHaveGetter(sourceProperty)
                        && IsHaveSetter(destinationProperty))
                    {
                        destinationProperty.SetValue(destination, sourceProperty.GetValue(
                            source, null), null);
                        break;
                    }
                }
            }
        }

        private static bool IsHaveGetter(PropertyInfo property)
        {
            return property
                .GetAccessors()
                .Any(accessor => accessor.ReturnType != typeof(void) && accessor.IsPublic) /*property.CanRead*/;
        }

        private static bool IsHaveSetter(PropertyInfo property)
        {
            return property
                .GetAccessors()
                .Any(accessor => accessor.ReturnType == typeof(void) && accessor.IsPublic) /*property.CanWrite*/;
        }
    }
}
