using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CameraCoolerGUI
{
    class Result<T> where T : struct
    {
        private T? resultObject;
        private string errorMessage;

        private Result(T? resultObject, string errorMessage)
        {
            this.resultObject = resultObject;
            this.errorMessage = errorMessage;
        }

        public static Result<T> Ok(T resultObject)
        {
            return new Result<T>(resultObject, null);
        }

        public static Result<T> Error(string errorMessage)
        {
            return new Result<T>(null, errorMessage);
        }

        public bool IsOk()
        {
            return resultObject != null;
        }

        public bool IsNotOk()
        {
            return !IsOk();
        }

        public T GetResultObject()
        {
            return resultObject.Value;
        }

        public string GetErrorMessage()
        {
            return errorMessage;
        }
    }
}
