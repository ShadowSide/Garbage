using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using Global.BusinessCommon.Helpers.ExceptionHelpers;

namespace Global.BusinessCommon.Helpers
{
    public static class Hasher
    {
        static public string Sha1(Stream stream, bool withoutDash = true, bool saveStreamPosition = true)
        {
            var savedPosition = saveStreamPosition && stream.CanSeek ? stream.Position : default(long?);
            using(new RAII(() =>
            {
                if (savedPosition.HasValue)
                    stream.Position = savedPosition.Value;
            }))
            using (SHA1 sha = new SHA1CryptoServiceProvider())
            {
                byte[] result = sha.ComputeHash(stream);
                var resultString = BitConverter.ToString(result);
                if (withoutDash)
                    resultString = resultString.Replace("-", String.Empty);
                return resultString;
            }
        }

        static public string Sha1(string str, bool withoutDash = true)
        {
            using (var stream = new MemoryStream(Encoding.Unicode.GetBytes(str)))
            {
                stream.Seek(0, SeekOrigin.Begin);
                return Sha1(stream, withoutDash:withoutDash, saveStreamPosition:false);
            }
        }
    }
}
