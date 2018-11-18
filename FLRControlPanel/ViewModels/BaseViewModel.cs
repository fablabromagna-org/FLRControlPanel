using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Runtime.CompilerServices;

using Xamarin.Forms;

using FLRControlPanel.Models;
using FLRControlPanel.Services;

namespace FLRControlPanel.ViewModels
{
    // Classe base per tutte le ViewModel.
    // Fornisce un sistema snello per creare delle Property bindabili nello XAML
    // Contiene il db (finto per ora)
    // Contiene un paio di property come il titolo della App 
    // e un bool IsBusy per attivare le progressbar sulle operazioni lente.
    public class BaseViewModel : INotifyPropertyChanged
    {
        public IDataStore<Item> DataStore => DependencyService.Get<IDataStore<Item>>() ?? new MockDataStore();

        bool isBusy = false;
        public bool IsBusy
        {
            get { return isBusy; }
            set { SetProperty(ref isBusy, value); }
        }

        string title = string.Empty;
        public string Title
        {
            get { return title; }
            set { SetProperty(ref title, value); }
        }

        string appName = string.Empty;
        public string AppName
        {
            get { return appName; }
            set { SetProperty(ref appName, value); }
        }

        // Il metodo SetProperty è un metodo generico <T> per essere riutilizzabile da tutti.
        //
        // Usa una serie di trucchetti...
        //
        // Usa [CallerMemberName]
        // che serve per settare il parametro del metodo al nome della property chiamante.
        //
        // Si applica solo a parametri che hanno un valore di default
        // (il valore di default si impone con = alla fine...)
        //
        // https://docs.microsoft.com/it-it/dotnet/api/system.runtime.compilerservices.callermembernameattribute?view=netframework-4.7.2
        //
        // 
        protected bool SetProperty<T>(ref T backingStore, T value,
            [CallerMemberName]string propertyName = "",
            Action onChanged = null)
        {
            // Per ottimizzare, se il nuovo valore è uguale al vecchio, non si fa nulla.
            // Nei metodi generici, la comparazione fatta così...
            // if( backingStore == value ) ...
            // da un errore di compilazione (non si possono confrontare due Tipi T)

            // si deve quindi usare EqualityComparer
            if (EqualityComparer<T>.Default.Equals(backingStore, value))
                return false;

            // Ora che è tutto a posto possiamo aggiornare il valore 
            // (che è passato per ref)
            backingStore = value;

            // volendo fa una callback ...
            onChanged?.Invoke();

            // poi propaga l'evento ai piani alti dello XAML (bubble)
            OnPropertyChanged(propertyName);

            return true;
        }

        public event PropertyChangedEventHandler PropertyChanged;
        protected void OnPropertyChanged([CallerMemberName] string propertyName = "")
        {
            var changed = PropertyChanged;
            if (changed == null)
                return;

            changed.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}
