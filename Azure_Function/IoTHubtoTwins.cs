using System;
using Azure;
using System.Net.Http;
using Azure.Core.Pipeline;
using Azure.DigitalTwins.Core;
using Azure.Identity;
using Microsoft.Azure.WebJobs;
using Microsoft.Azure.WebJobs.Extensions.EventGrid;
using Microsoft.Extensions.Logging;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using Azure.Messaging.EventGrid;

namespace IotHubtoTwins
{
    public class IoTHubtoTwins
    {
        private static readonly string adtInstanceUrl = Environment.GetEnvironmentVariable("ADT_SERVICE_URL");
        private static readonly HttpClient httpClient = new HttpClient();

        [FunctionName("IoTHubtoTwins")]
        // While async void should be used with caution, it's not uncommon for Azure function, since the function isn't awaiting the task.
        #pragma warning disable AZF0001 // Suppress async void error
        public async void Run([EventGridTrigger] EventGridEvent eventGridEvent, ILogger log)
        #pragma warning restore AZF0001 // Suppress async void error
        {
            if (adtInstanceUrl == null) log.LogError("Application setting \"ADT_SERVICE_URL\" not set");

            try
            {
                // Authenticate with Digital Twins
                var cred = new DefaultAzureCredential();
                var client = new DigitalTwinsClient(new Uri(adtInstanceUrl), cred);
                log.LogInformation($"ADT service client connection created.");
            
                if (eventGridEvent != null && eventGridEvent.Data != null)
                {
                    log.LogInformation(eventGridEvent.Data.ToString());

                    // <Find_device_ID_temperature_and_humidity>
                    JObject deviceMessage = (JObject)JsonConvert.DeserializeObject(eventGridEvent.Data.ToString());
                    string deviceId = (string)deviceMessage["systemProperties"]["iothub-connection-device-id"];
                    var temperature = deviceMessage["body"]["decoded"]["payload"]["generator_temperature"];
                    log.LogInformation($"Device:{deviceId} Temperature is: {temperature}");
                    var humidity = deviceMessage["body"]["decoded"]["payload"]["generator_humidity"];
                    log.LogInformation($"Device:{deviceId} Humidity is: {humidity}");

                    // <Update_twin_with_device_temperature>
                    var updateTwinData = new JsonPatchDocument();
                    updateTwinData.AppendReplace("/Temperature", temperature.Value<double>());
					updateTwinData.AppendReplace("/Humidity", humidity.Value<double>());
                    await client.UpdateDigitalTwinAsync(deviceId, updateTwinData);
                    // </Update_twin_with_device_temperature>
                }
            }
            catch (Exception ex)
            {
                log.LogError($"Error in ingest function: {ex.Message}");
            }
        }
    }
}