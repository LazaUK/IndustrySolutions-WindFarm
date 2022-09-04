# Industry Solutions: Wind Farm's health monitoring solutions
Wind farms are great contributors to our sustainability agenda as generators of a green renewable energy. They are typically located in remote onshore / offshore locations, which makes it crucial to monitor their health and provide timely support.

In this repo, you will find a detailed step-by-step guide on how to enable ingestion of remote telemetry in Azure, update of wind farm's Azure digital twin (ADT) and real-time health monitoring in ADT's 3D scene.

## Step 1 - Setup Azure Digital Twins
1. In [Azure portal](https://portal.azure.com) provision new instance of **Azure Digital Twins** (ADT)
2. Create new **Azure Storage** account and enable access to it from ADT with the following AZ command
```
az storage cors add --services b --methods GET OPTIONS POST PUT --origins https://explorer.digitaltwins.azure.net --allowed-headers Authorization x-ms-version x-ms-blob-type --account-name <YOUR_STORAGE_ACCOUNT>
```

8. Now from the left navigation bar select **JSON Editor**, then replace schema definition on the right with content from Git-attached **alexaskill-nodejs/model.json** file. ![screenshot_1.8](images/screenshot_step1.8.png)
