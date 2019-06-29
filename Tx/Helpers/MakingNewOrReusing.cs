using GlobalInterfaces.Containers.MakingNewOrReusing;
using System;

namespace Global.BusinessCommon.Helpers.Containers.MakingNewOrReusing
{
    /// <summary>
    /// Класс методов генераторов для создания хранящей фабрики IDisposable объектов.
    /// Обычно используют для настраивания владения объектами контекстов конекта к бд. 
    /// Особенно удобно для юнит тестирования с моканьем БД.
    /// Класс потребитель этой фабрики принимает в методе или конструкторе интерфейс хранящей фабрики
    /// и это удобно для удобного настраивания владения (ответственного за вызов Dispose) снаруже класса потребителя.
    /// При этом для вызывающего класса всё прозрачно и в любом случае он ведет себя одинаково.
    /// Смотри описание StoreFactoryMode или DisposableStoreFactoryMode.
    /// </summary>
    public static class StoreFactoryMaker
    {
        /// <summary>
        /// Создает хранящую фабрику из значения value.
        /// Смотри описание StoreFactoryMode
        /// Параметр mode фиктивный только ради перегрузки по типу, обрабатывая только StoreFactoryMode.StoringValueAndMakerWillDisposingValue
        /// </summary>
        public static IStoreFactory<T> Make<T>(T value, StoreFactoryMode mode = StoreFactoryMode.StoringValueAndMakerWillDisposingValue) where T : class, IDisposable
        {
            switch (mode)
            {
                case StoreFactoryMode.StoringValueAndMakerWillDisposingValue:
                    return new Impl.Stored<T>(value);
                case StoreFactoryMode.MakingNewValue:
                    throw new ArgumentException($"Logic error. Not allowed {nameof(mode)}=${mode}. Value already created. Can't making new from already created.");
            }
            throw new NotImplementedException($"Not implemented {nameof(mode)}=${mode}.");
        }

        /// <summary>
        /// Создает хранящую фабрику из функтора valueFactory.
        /// Смотри описание StoreFactoryMode
        /// </summary>
        public static IStoreFactory<T> Make<T>(Func<T> valueFactory, StoreFactoryMode mode = StoreFactoryMode.MakingNewValue) where T : class, IDisposable
        {
            switch (mode)
            {
                case StoreFactoryMode.StoringValueAndMakerWillDisposingValue:
                    return new Impl.Stored<T>(valueFactory is null ? throw new ArgumentNullException($"Not supported null {nameof(valueFactory)}") : valueFactory());
                case StoreFactoryMode.MakingNewValue:
                    return new Impl.Factory<T>(valueFactory);
            }
            throw new NotImplementedException($"Not implemented {nameof(mode)}=${mode}.");
        }

        /// <summary>
        /// Создает хранящую фабрику из значения value.
        /// Смотри описание DisposableStoreFactoryMode
        /// </summary>
        public static IDisposableStoreFactory<T> MakeDisposable<T>(T value, DisposableStoreFactoryMode mode = DisposableStoreFactoryMode.StoringValueAndMakerWillDisposingValue) where T : class, IDisposable
        {
            switch (mode)
            {
                case DisposableStoreFactoryMode.StoringAndDisposingValue:
                    return new Impl.DisposableStoreFactory<T>(value);
                case DisposableStoreFactoryMode.StoringValueAndMakerWillDisposingValue:
                    return new Impl.NotNeedDisposeStoreFactoryAdapter<T>(new Impl.Stored<T>(value));
                case DisposableStoreFactoryMode.MakingNewValue:
                    throw new ArgumentException($"Logic error. Not allowed {nameof(mode)}=${mode}. Value already created. Can't making new from already created.");
            }
            throw new NotImplementedException($"Not implemented {nameof(mode)}=${mode}.");
        }

        /// <summary>
        /// Создает хранящую фабрику из функтора valueFactory.
        /// Смотри описание DisposableStoreFactoryMode
        /// </summary>
        public static IDisposableStoreFactory<T> MakeDisposable<T>(Func<T> valueFactory, DisposableStoreFactoryMode mode = DisposableStoreFactoryMode.MakingNewValue) where T : class, IDisposable
        {
            switch (mode)
            {
                case DisposableStoreFactoryMode.StoringAndDisposingValue:
                    return new Impl.DisposableStoreFactory<T>(valueFactory());
                case DisposableStoreFactoryMode.StoringValueAndMakerWillDisposingValue:
                    return new Impl.NotNeedDisposeStoreFactoryAdapter<T>(new Impl.Stored<T>(valueFactory is null ? throw new ArgumentNullException($"Not supported null {nameof(valueFactory)}") : valueFactory()));
                case DisposableStoreFactoryMode.MakingNewValue:
                    return new Impl.NotNeedDisposeStoreFactoryAdapter<T>(new Impl.Factory<T>(valueFactory));
            }
            throw new NotImplementedException($"Not implemented {nameof(mode)}=${mode}.");
        }

        /// <summary>
        /// Создает хранящую фабрику из значения value.
        /// Смотри описание DisposableStoreFactoryMode
        /// </summary>
        public static IDisposableStoreFactory<T> Make<T>(T value, DisposableStoreFactoryMode mode) where T : class, IDisposable
        {
            return MakeDisposable(value, mode);
        }

        /// <summary>
        /// Создает хранящую фабрику из функтора valueFactory.
        /// Смотри описание DisposableStoreFactoryMode
        /// </summary>
        public static IDisposableStoreFactory<T> Make<T>(Func<T> valueFactory, DisposableStoreFactoryMode mode) where T : class, IDisposable
        {
            return MakeDisposable(valueFactory, mode);
        }

        /// <summary>
        /// Конвертер StoreFactoryMode в DisposableStoreFactoryMode, тк смысл один и тоже, но 
        /// </summary>
        public static DisposableStoreFactoryMode ToDisposableStoreFactoryMode(this StoreFactoryMode mode)
        {
            switch (mode)
            {
                case StoreFactoryMode.MakingNewValue:
                    return DisposableStoreFactoryMode.MakingNewValue;
                case StoreFactoryMode.StoringValueAndMakerWillDisposingValue:
                    return DisposableStoreFactoryMode.StoringValueAndMakerWillDisposingValue;
            }
            throw new NotImplementedException($"Not implemented {nameof(mode)}=${mode}.");
        }
    }

    namespace Impl
    {
        public class Stored<T> : IStoreFactory<T> where T : class, IDisposable//-V3072
            //-V3072
        {
            private readonly T _value;

            public Stored(T value)
            {
                _value = value ?? throw new ArgumentNullException($"Not supported null for stored {nameof(value)}");
            }

            public IStoreFactoryValue<T> ValueContainer => new StoredValue<T>(_value);
        }

        public class Factory<T> : IStoreFactory<T> where T : class, IDisposable//-V3072
            //-V3072
        {
            private readonly Func<T> _valueFactory;

            public Factory(Func<T> valueFactory)
            {
                _valueFactory = valueFactory ?? throw new ArgumentNullException($"Not supported null for {nameof(valueFactory)}");
            }

            public IStoreFactoryValue<T> ValueContainer => new StoredDisposableValue<T>(_valueFactory() ?? throw new InvalidOperationException("Not supported null returning factory method"));
        }

        public class DisposableStoreFactory<T> : IDisposableStoreFactory<T> where T : class, IDisposable//-V3072
            //-V3072
        {
            private T _value;

            public DisposableStoreFactory(T value)
            {             
                _value = value?? throw new ArgumentNullException($"Not supported null for stored {nameof(value)}");
            }

            public IStoreFactoryValue<T> ValueContainer => new StoredValue<T>(_value ?? throw new ObjectDisposedException(nameof(ValueContainer)));

            public void Dispose()
            {
                var value = _value;
                _value = null;
                value?.Dispose();
            }
        }

        public class StoredValue<T> : IStoreFactoryValue<T> where T : class, IDisposable
        {
            private T _value;

            public StoredValue(T value)
            {
                _value = value ?? throw new ArgumentNullException($"Internal logic error. Not supported null for stored {nameof(value)}");
            }

            public T Value => _value ?? throw new ObjectDisposedException(nameof(Value));

            public void Dispose()
            {
                _value = null;
            }
        }

        public class StoredDisposableValue<T> : IStoreFactoryValue<T> where T : class, IDisposable
        {
            private T _value;

            public StoredDisposableValue(T value)
            {
                _value = value ?? throw new ArgumentNullException($"Internal logic error. Not supported null for stored {nameof(value)}");
            }

            public T Value => _value ?? throw new ObjectDisposedException(nameof(Value));

            public void Dispose()
            {
                var value = _value;
                _value = null;
                value?.Dispose();
            }
        }

        public class NotNeedDisposeStoreFactoryAdapter<T> : IDisposableStoreFactory<T> where T : class, IDisposable
        {
            private IStoreFactory<T> _storeFactory;

            public NotNeedDisposeStoreFactoryAdapter(IStoreFactory<T> storeFactory)
            {
                _storeFactory = storeFactory ?? throw new ArgumentNullException($"Internal logic error. Not supported null for {nameof(storeFactory)}");
            }

            public IStoreFactoryValue<T> ValueContainer => _storeFactory?.ValueContainer ?? throw new ObjectDisposedException(nameof(ValueContainer));

            public void Dispose()
            {
                _storeFactory = null;
            }
        }
    }
}
