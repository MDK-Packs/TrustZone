# Over-the-Air Updates

Over-the-air (OTA) updates allow you to deploy firmware updates to one or more devices in your fleet.  

More information is available here:  
https://docs.aws.amazon.com/freertos/latest/userguide/freertos-ota-dev.html

## Setup code signing key

The device uses digital signatures to verify the authenticity of the firmware updates sent Over-The-Air.
Images are signed by an authorized source who creates the image, and device can verify the signature of the image, 
using the corresponding public key of the source. 
Steps below shows how to setup and provision the code signing credentials to enable the Cloud to digitally sign 
the image and the device to verify the image signature before boot.

Generate a Code Signing key:
1. In your working directory, use the following text to create a file named **cert_config.txt**. 
   Replace test_signer@amazon.com with your email address:
   ```
   [ req ]
   prompt             = no
   distinguished_name = my_dn
    
   [ my_dn ]
   commonName         = test_signer@amazon.com
    
   [ my_exts ]
   keyUsage           = digitalSignature
   extendedKeyUsage   = codeSigning
   ```

2. Create an ECDSA code-signing private key:
   ```
   openssl genpkey -algorithm EC -pkeyopt ec_paramgen_curve:P-256 -pkeyopt ec_param_enc:named_curve -outform PEM -out ecdsasigner-priv-key.pem
   ```

3. Generate the corresponding public key from the private key:
   ```
   openssl ec -inform pem -in ecdsasigner-priv-key.pem -pubout -outform pem -out ecdsasigner-pub-key.pem
   ```

4. Create an ECDSA code-signing certificate to be uploaded to the AWS Certificate Manager (ACM) service:
   ```
   openssl req -new -x509 -config cert_config.txt -extensions my_exts -nodes -days 365 -key ecdsasigner-priv-key.pem -out ecdsasigner.crt
   ```

5. Import the code-signing certificate and private key into AWS Certificate Manager (ACM):
   ```
   aws acm import-certificate --certificate fileb://ecdsasigner.crt --private-key fileb://ecdsasigner-priv-key.pem
   ```

   > Note: This command displays an ARN for your certificate. You will need this ARN when you create an OTA update job later.

6. Create a signing profile in AWS to sign the firmware image:
   ```
   aws signer put-signing-profile --profile-name <your signing profile name> --signing-material certificateArn=<certificate arn created in step 4> --platform AmazonFreeRTOS-Default --signing-parameters certname=ota_signer_pub
   ```

7. Import public key to the target device.  
   Run the provision application and connect to the target device via a serial terminal (baudrate = 115200).  
   On the command line prompt, type following command to import the OTA signing key:  

   ```
   pki import key ota_signer_pub
   ```

   Then copy paste the `ecdsasigner-pub-key.pem` into the terminal like shown below:  
   ```
   -----BEGIN PUBLIC KEY-----
   XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
   XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX=
   -----END PUBLIC KEY-----
   ```

   You should get information from the device as below:
   ```
   Success: Public Key loaded to label: 'ota_signer_pub'.
   ```

   > Note: `ota_signer_pub` is the label used to refer to the code signing key during verification of the firmware update.

## Setup OTA S3 bucket, Service role and policies in AWS

1. S3 bucket is used to store the new firmware image to be updated. To create a new S3 bucket follow these steps here:  
   https://docs.aws.amazon.com/freertos/latest/userguide/dg-ota-bucket.html

2. Create a service role which grants permission for OTA service to access the firmware image:  
   https://docs.aws.amazon.com/freertos/latest/userguide/create-service-role.html

3. Create an OTA update policy using the documentation here:  
   https://docs.aws.amazon.com/freertos/latest/userguide/create-ota-user-policy.html

4. Add a policy for AWS IoT to access the code signing profile:  
   https://docs.aws.amazon.com/freertos/latest/userguide/code-sign-policy.html

## Run OTA firmware update job

1. Bump up the version of the new firmware image to be updated.  

   Firmware version (Major, Minor and Build) is defined in the OTA Demo configuration file **ota_demo_config.h**.  
   Increase the version of the updated firmware (ex: 0.9.3, default version is 0.9.2) and build it.

2. Sign the image using the **sign_image.bat** script and provide the increased **version** and also increment the **security counter** (initial value is 1):  

   ```
   sign_image <name>.hex <version> <security_counter>
   ```

   This will create a signed binary image`<name>_ota.bin` to be verified by the Bootloader and deployed - see next step.

3. Create a new OTA Update job using Amazon AWS Cloud Console:

   - Open **IoT Core** on the Amazon AWS Could Console
   - Under **Manage - Remote actions - Jobs** click on the **Create** button
   - Under **Create job** select **Create FreeRTOS OTA update job** and click on the **Next** button
   - Under **OTA job properties** enter job name (for example FUOTA_B_U585I)
   - Under **OTA file configuration** set:
      - Under **Devices - Device to update** select your Thing
      - Under **Devices - Select the protocol for file transfer** select the **MQTT**
      - Under **File - Sign and choose your file** select the **Sign a new file for me**
      - Under **File - Code signing profile - Existing code signing profile** select the previously created **ota_signer_pub**
      - Under **File - File** select **Upload a new file.**
      - Under **File - File - File to upload** click on the **Choose file** and select the previously generated `<name>_ota.bin` file
      - Under **File upload location in S3 - S3 URL**  select the previously created s3 bucket (for example s3://s3-bucket-b-u585i)
      - Under **File upload location in S3 - Path name of file on device**  enter **non_secure image** (for secure image update **secure image**)
      - Under **IAM role - Role** select previously created role for the OTA (for example role-ota-update)
      - Click on the **Next** button
   - Under **OTA job configuration** select the **Your job will complete after deploying to the devices and groups that you chose (snapshot)** type
   - Click on the **Create job** button

#### Monitoring and Verification of firmware update

1. Once the job is created, in the terminal, you will see that OTA job is accepted and device starts downloading the selected image.
   ```
   <INF>  2701148 [OTAAgent  ] Job document was accepted. Attempting to begin the update. (ota.c:2206)
   <INF>  2701148 [OTAAgent  ] Job parsing success: OtaJobParseErr_t=OtaJobParseErrNone, Job name=AFR_OTA-fuota_b_u585i (ota.c:2328)
   <WRN>  2701148 [OTAAgent  ] Received an unhandled callback event from OTA Agent, event = 6 (ota_update_task.c:670)
   <INF>  2701149 [OTAAgent  ] Setting OTA data interface. (ota.c:938)
   <INF>  2701149 [OTAAgent  ] Current State=[CreatingFile], Event=[ReceivedJobDocument], New state=[CreatingFile] (ota.c:2846)
   ```

2. Once the full image has been downloaded, the OTA library verifies the image signature and activates the new image in the unused flash bank.
   ```
   <INF>  2725716 [OTAAgent  ] Received entire update and validated the signature. (ota.c:2666)
   ```

3. New image boots up and performs a self-test, here it checks the version is higher than previous. If so it sets the new image as valid.
   ```
   <INF>    11125 [OTAAgent  ] New image validation succeeded in self test mode. (ota_update_task.c:640)
   ```

4. Checking the job status, should show the job as **Completed**

  - Open **IoT Core** on the Amazon AWS Could Console
  - Under **Manage - Remote actions - Jobs** check status of your OTA job
