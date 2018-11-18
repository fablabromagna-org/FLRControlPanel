using System;
using System.Windows.Input;

using Xamarin.Forms;

namespace FLRControlPanel.ViewModels
{
    public class AboutViewModel : BaseViewModel
    {
        public AboutViewModel()
        {
            Title = "About";
            AppName = "FLR Control Panel";

            // Questo è un comando che lato XAML può essere bindato in questo modo
            // <Button Text="Learn more" Command="{Binding OpenWebCommand}"/>
            OpenWebCommand = new Command( () => 
                Device.OpenUri ( new Uri("https://xamarin.com/platform"))
            );
        }

        public ICommand OpenWebCommand { get; }
    }
}