using System;
using System.Collections.Generic;
using System.Net.Http;
using Newtonsoft.Json.Linq;
using Xamarin.Forms;

namespace FLRControlPanel.Views
{
    public partial class IoTPage : ContentPage
    {
        public IoTPage()
        {
            InitializeComponent();
            slider.Value = 100;
        }

        private void slider_ValueChanged(object sender, ValueChangedEventArgs e)
        {
            //lastTemperature = e.NewValue;
            slider.Value = Math.Round(e.NewValue, 2);
        }

        async void Load(object sender, System.EventArgs e)
        {
            try
            {
                slider.IsEnabled = false;

                string IoT = "lab226";

                // Legge
                // https://dweet.io/get/latest/dweet/for/lab225

                // Se la IoT non esiste riponde così
                // { "this":"failed","with":404,"because":"we couldn't find this"}


                HttpClient client = new HttpClient();
                var r = await client.GetAsync($"https://dweet.io/get/latest/dweet/for/{IoT}");
                string result = await r.Content.ReadAsStringAsync();

                JObject rss = JObject.Parse(result);

                if (rss["this"].ToString() != "failed")
                {
                    // IoT Esiste

                    // Legge dal web
                    //slider.Value = (double)rss["with"][0]["content"]["temperatura"];
                    //slider.Value = double.Parse(
                    //    rss["with"][0]["content"]["temperatura"].ToString(), 
                    //    System.Globalization.NumberStyles.Any);

                    double valore;
                    string strValore = rss["with"][0]["content"]["temperatura"].ToString();

                    strValore = strValore.Replace(',', '.');
                    Double.TryParse(strValore, out valore);
                    valore = Math.Round(valore,2);
                    slider.Value = valore;

                    lblErrori.BackgroundColor = Color.Transparent;
                }
                else
                {
                    throw new Exception($"IoT {IoT} non trovata!");
                }

            }
            catch (Exception Err)
            {
                lblErrori.Text = Err.Message;
                lblErrori.BackgroundColor = Color.Red;
            }

            slider.IsEnabled = true;
        }

        async void Save(object sender, System.EventArgs e)
        {
            slider.IsEnabled = false;

            try
            {
                // Scrive
                // https://dweet.io/dweet/for/lab225?temperatura=100

                HttpClient client = new HttpClient();
                var r = await client.GetAsync($"https://dweet.io/dweet/for/lab226?temperatura={slider.Value}");
                string result = await r.Content.ReadAsStringAsync();

                JObject rss = JObject.Parse(result);

                /* { 
                     "this":"succeeded",
                     "by":"dweeting",
                     "the":"dweet",
                     "with":
                     { 
                        "thing":"lab225",   
                        "created":"2018-10-20T11:30:58.360Z",
                        "content":{ "temperatura":100},
                        "transaction":"8550e925-9e39-4219-ab1e-82f4c3b7daf4"
                     } 
                   }
                */

                // slider.Value = (double)rss["with"]["content"]["temperatura"];
                lblErrori.Text = "";
            }
            catch (Exception Err) { lblErrori.Text = Err.Message; }

            slider.IsEnabled = true;
        }

    }
}
