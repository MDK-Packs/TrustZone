# Provisioning the target

The **PROVISIONING** process does the following:
  - Sets a **Thing Name** on the device
  - Sets the **WiFi SSID** and **Wifi password** on the device (for connecting to the network via the WiFi)
  - Sets the **MQTT endpoint** on the device
  - Generates ECDSA device **key pair**
  - Generates a self-signed **certificate**
  - **Registers** the previously generated self-signed **certificate** with the **IoT Core** service on the Amazon AWS **Cloud**
  - **Registers** the **Thing Name** on the Amazon AWS **Cloud**
  - **Attaches** the **Certificate** to the thing on the Amazon AWS **Cloud**
  - **Registers** a **Policy** for the thing on the Amazon AWS **Cloud**
  - **Attaches** the **Policy** to the thing on the Amazon AWS **Cloud**

After provisioning is done the device can connect to the Amazon AWS Cloud.

## Prerequisites

  - [python v3.10 or later](https://www.python.org/downloads/windows/) (with pip)
  - [AWS CLI2](https://awscli.amazonaws.com/AWSCLIV2.msi)

**Python requirements**

  - installed **imgtool v1.9.0**, **boto3 v1.22.0**, **requests**, **pyserial** modules  
    to install execute the following command in Git Bash shell:
    ```
    pip install imgtool==1.9.0 boto3==1.22.0 requests pyserial
    ```

**AWS CLI2 requirements**

  - **Amazon AWS Cloud account**, if you do not have one please follow the documentation to 
    [create and activate the Amazon AWS Cloud account](https://aws.amazon.com/premiumsupport/knowledge-center/create-and-activate-aws-account/)

  - Enable AWS CLI access following the [official documentation](https://docs.aws.amazon.com/cli/latest/userguide/cli-configure-quickstart.html) 
    or alternatively following the steps below:
    - [Log into Amazon AWS Cloud Console](https://console.aws.amazon.com/)
    - open **IAM** services
    - under **Access management** select **Users**
    - select desired user
    - select **Security credentials** tab
    - under **Access key** section click on the **Create access** key button
    - download the created **.csv** file or store your **Access key ID** and **Secret access key** locally
    - from Windows Command Shell execute the following command:
    ```
    aws configure
    ```
     - Fill in the required fields (first two as retrieved in previous steps): 
    ```
    AWS Access Key ID [****************xxxx]: Your access key ID
    AWS Secret Access Key [****************xxxx]: Your Secret access key
    Default region name [eu-central-1]: Your default region
    Default output format [json]: json
    ```

## Provision the target

To provision the target run the Provision application in the target and start the **provision.py** script by executing the following command:
```
provision.py --interactive
```

proceed entering required data in serial terminal window:
```
Target device path: COM5
Connecting to target...
[ INFO ] Found credentials in shared credentials file: ~/.aws/credentials (credentials.py:load)
Interactive configuration mode: Press return to use defaults (displayed in brackets)
time_hwm[1651013601]: <return>
mqtt_port[8883]: <return>
wifi_ssid[]: wifi_ssid<return>
wifi_credential[]: wifi_password<return>
mqtt_endpoint[xxxxxxxxxxxxxx-ats.iot.xxxxxxxxx.amazonaws.com]: xxxxxxxxxxxxxx-ats.iot.xxxxxxxxx.amazonaws.com<return>
thing_name[xxxxxxxxxxxxxxxx]: Thing_B_U585I<return>
Commiting target configuration...
Generating a new public/private key pair
Generating a self-signed Certificate
Attaching thing: xxxxxxxxxxxxxxxx to principal: arn:aws:iot:xxxxxxxxx:xxxxxxxxxxxxxx:cert/xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
Attaching the "AllowAllDev" policy to the device certificate.
Importing root ca certificate: "Starfield Services Root Certificate Authority - G2"
Provisioning process complete. Resetting target device...
```
> Note: **mqtt_endpoint** can be found in the Amazon AWS Cloud console in **IoT Core** service under [Settings](https://console.aws.amazon.com/iot/home#/settings)

