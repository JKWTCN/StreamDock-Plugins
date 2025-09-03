#include "HSDSwitchAudioAction.h"

#include "StreamDockCPPSDK/StreamDockSDK/NlohmannJSONUtils.h"
#include "StreamDockCPPSDK/StreamDockSDK/HSDLogger.h"
#include <string>


//windows

#include "Mmdeviceapi.h"
#include "PolicyConfig.h"
#include <propkey.h>
#include <iostream>
#include <fstream>
#include <vector>

#include <locale>
#include <codecvt>

#include <thread>



#ifndef PKEY_Device_FriendlyName
DEFINE_PROPERTYKEY(PKEY_Device_FriendlyName, 0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 14);
#endif

//�л�Ĭ����Ƶ����豸
void SwitchDefaultAudioPlaybackDevice(const std::wstring& deviceId) {
    HRESULT hr = CoInitialize(nullptr);
    if (FAILED(hr))
    {
        return;
    }

    IMMDeviceEnumerator* pEnumerator = nullptr;
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&pEnumerator));
    if (FAILED(hr))
    {
        CoUninitialize();
        return;
    }

    IMMDeviceCollection* pCollection = nullptr;
    hr = pEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pCollection);
    if (FAILED(hr))
    {
        pEnumerator->Release();
        CoUninitialize();
        return;
    }

    UINT count;
    hr = pCollection->GetCount(&count);
    if (FAILED(hr))
    {
        pCollection->Release();
        pEnumerator->Release();
        CoUninitialize();
        return;
    }

    for (UINT i = 0; i < count; ++i)
    {
        IMMDevice* pDevice = nullptr;
        hr = pCollection->Item(i, &pDevice);
        if (FAILED(hr))
        {
            continue;
        }

        LPWSTR pwszID = nullptr;
        hr = pDevice->GetId(&pwszID);
        if (FAILED(hr))
        {
            pDevice->Release();
            continue;
        }

        if (deviceId == pwszID)
        {
            IPolicyConfigVista* pPolicyConfig;
            ERole reserved = eConsole;

            hr = CoCreateInstance(__uuidof(CPolicyConfigVistaClient),
                NULL, CLSCTX_ALL, __uuidof(IPolicyConfigVista), (LPVOID*)&pPolicyConfig);
            if (SUCCEEDED(hr))
            {
                hr = pPolicyConfig->SetDefaultEndpoint(pwszID, reserved);
                pPolicyConfig->Release();
            }

            CoTaskMemFree(pwszID);
            pDevice->Release();
            break;
        }

        CoTaskMemFree(pwszID);
        pDevice->Release();
    }

    pCollection->Release();
    pEnumerator->Release();
    CoUninitialize();
}

//��ȡ������Ƶ����豸
std::map<std::wstring, std::wstring> GetAllAudioPlaybackDevices() {
    std::map<std::wstring, std::wstring> deviceMap;

    HRESULT hr = CoInitialize(nullptr);
    if (FAILED(hr)) {
        return deviceMap;
    }

    IMMDeviceEnumerator* pEnumerator = nullptr;
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&pEnumerator));
    if (FAILED(hr)) {
        CoUninitialize();
        return deviceMap;
    }

    IMMDeviceCollection* pCollection = nullptr;
    hr = pEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pCollection);
    if (FAILED(hr)) {
        pEnumerator->Release();
        CoUninitialize();
        return deviceMap;
    }

    UINT count;
    hr = pCollection->GetCount(&count);
    if (FAILED(hr)) {
        pCollection->Release();
        pEnumerator->Release();
        CoUninitialize();
        return deviceMap;
    }

    for (UINT i = 0; i < count; ++i) {
        IMMDevice* pDevice = nullptr;
        hr = pCollection->Item(i, &pDevice);
        if (FAILED(hr)) {
            continue;
        }

        LPWSTR pwszID = nullptr;
        hr = pDevice->GetId(&pwszID);
        if (FAILED(hr)) {
            pDevice->Release();
            continue;
        }

        IPropertyStore* pProps = nullptr;
        hr = pDevice->OpenPropertyStore(STGM_READ, &pProps);
        if (FAILED(hr)) {
            CoTaskMemFree(pwszID);
            pDevice->Release();
            continue;
        }

        PROPVARIANT varName;
        PropVariantInit(&varName);

        hr = pProps->GetValue(PKEY_Device_FriendlyName, &varName);
        if (SUCCEEDED(hr)) {
            std::wstring deviceId(pwszID);
            std::wstring deviceName(varName.pwszVal);

            deviceMap[deviceName] = deviceId;
        }

        PropVariantClear(&varName);
        pProps->Release();
        CoTaskMemFree(pwszID);
        pDevice->Release();
    }

    pCollection->Release();
    pEnumerator->Release();
    CoUninitialize();

    return deviceMap;
}


//�л�Ĭ����Ƶ�����豸
void SwitchDefaultAudioCaptureDevice(const std::wstring& deviceId) {
    HRESULT hr = CoInitialize(nullptr);
    if (FAILED(hr))
    {
        return;
    }

    IMMDeviceEnumerator* pEnumerator = nullptr;
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&pEnumerator));
    if (FAILED(hr))
    {
        CoUninitialize();
        return;
    }

    IMMDeviceCollection* pCollection = nullptr;
    hr = pEnumerator->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &pCollection);
    if (FAILED(hr))
    {
        pEnumerator->Release();
        CoUninitialize();
        return;
    }

    UINT count;
    hr = pCollection->GetCount(&count);
    if (FAILED(hr))
    {
        pCollection->Release();
        pEnumerator->Release();
        CoUninitialize();
        return;
    }

    for (UINT i = 0; i < count; ++i)
    {
        IMMDevice* pDevice = nullptr;
        hr = pCollection->Item(i, &pDevice);
        if (FAILED(hr))
        {
            continue;
        }

        LPWSTR pwszID = nullptr;
        hr = pDevice->GetId(&pwszID);
        if (FAILED(hr))
        {
            pDevice->Release();
            continue;
        }

        if (deviceId == pwszID)
        {
            IPolicyConfigVista* pPolicyConfig;
            ERole reserved = eConsole;

            hr = CoCreateInstance(__uuidof(CPolicyConfigVistaClient),
                NULL, CLSCTX_ALL, __uuidof(IPolicyConfigVista), (LPVOID*)&pPolicyConfig);
            if (SUCCEEDED(hr))
            {
                hr = pPolicyConfig->SetDefaultEndpoint(pwszID, reserved);
                pPolicyConfig->Release();
            }

            CoTaskMemFree(pwszID);
            pDevice->Release();
            break;
        }

        CoTaskMemFree(pwszID);
        pDevice->Release();
    }

    pCollection->Release();
    pEnumerator->Release();
    CoUninitialize();
}


//��ȡ������Ƶ�����豸
std::map<std::wstring, std::wstring> GetAllAudioCaptureDevices() {
    std::map<std::wstring, std::wstring> deviceMap;

    HRESULT hr = CoInitialize(nullptr);
    if (FAILED(hr)) {
        return deviceMap;
    }

    IMMDeviceEnumerator* pEnumerator = nullptr;
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&pEnumerator));
    if (FAILED(hr)) {
        CoUninitialize();
        return deviceMap;
    }

    IMMDeviceCollection* pCollection = nullptr;
    hr = pEnumerator->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &pCollection);
    if (FAILED(hr)) {
        pEnumerator->Release();
        CoUninitialize();
        return deviceMap;
    }

    UINT count;
    hr = pCollection->GetCount(&count);
    if (FAILED(hr)) {
        pCollection->Release();
        pEnumerator->Release();
        CoUninitialize();
        return deviceMap;
    }

    for (UINT i = 0; i < count; ++i) {
        IMMDevice* pDevice = nullptr;
        hr = pCollection->Item(i, &pDevice);
        if (FAILED(hr)) {
            continue;
        }

        LPWSTR pwszID = nullptr;
        hr = pDevice->GetId(&pwszID);
        if (FAILED(hr)) {
            pDevice->Release();
            continue;
        }

        IPropertyStore* pProps = nullptr;
        hr = pDevice->OpenPropertyStore(STGM_READ, &pProps);
        if (FAILED(hr)) {
            CoTaskMemFree(pwszID);
            pDevice->Release();
            continue;
        }

        PROPVARIANT varName;
        PropVariantInit(&varName);

        hr = pProps->GetValue(PKEY_Device_FriendlyName, &varName);
        if (SUCCEEDED(hr)) {
            std::wstring deviceId(pwszID);
            std::wstring deviceName(varName.pwszVal);

            deviceMap[deviceName] = deviceId;
        }

        PropVariantClear(&varName);
        pProps->Release();
        CoTaskMemFree(pwszID);
        pDevice->Release();
    }

    pCollection->Release();
    pEnumerator->Release();
    CoUninitialize();

    return deviceMap;
}

//��ȡ��ǰ����ʹ�õ�����豸
nlohmann::json GetActiveAudioPlaybackDevice() {
    nlohmann::json jsonResult;

    HRESULT hr = CoInitialize(nullptr);
    if (FAILED(hr)) {
        return jsonResult;
    }

    IMMDeviceEnumerator* pEnumerator = nullptr;
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&pEnumerator));
    if (FAILED(hr)) {
        CoUninitialize();
        return jsonResult;
    }

    IMMDevice* pDevice = nullptr;
    hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
    if (FAILED(hr)) {
        pEnumerator->Release();
        CoUninitialize();
        return jsonResult;
    }

    LPWSTR pwszID = nullptr;
    hr = pDevice->GetId(&pwszID);
    if (FAILED(hr)) {
        pDevice->Release();
        pEnumerator->Release();
        CoUninitialize();
        return jsonResult;
    }

    IPropertyStore* pProps = nullptr;
    hr = pDevice->OpenPropertyStore(STGM_READ, &pProps);
    if (FAILED(hr)) {
        CoTaskMemFree(pwszID);
        pDevice->Release();
        pEnumerator->Release();
        CoUninitialize();
        return jsonResult;
    }

    PROPVARIANT varName;
    PropVariantInit(&varName);

    hr = pProps->GetValue(PKEY_Device_FriendlyName, &varName);
    if (SUCCEEDED(hr)) {
        jsonResult["deviceName"] = varName.pwszVal;
        jsonResult["deviceId"] = pwszID;
    }

    PropVariantClear(&varName);
    pProps->Release();
    CoTaskMemFree(pwszID);
    pDevice->Release();
    pEnumerator->Release();
    CoUninitialize();

    return jsonResult;
}

std::string WStringToString(const std::wstring& wstr) {
    // ʹ�� std::wstring_convert ����ת��
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    return converter.to_bytes(wstr);
}

std::wstring StringToWString(const std::string& str) {
    // ʹ�� std::wstring_convert ����ת��
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(str);
}

void HSDExampleAction::DidReceiveSettings(const nlohmann::json& payload) {
    HSDLogger::LogMessage("DidReceiveSettings");
}

void HSDExampleAction::KeyDown(const nlohmann::json& payload) {
    HSDLogger::LogMessage("KeyDown");
    //if (NlohmannJSONUtils::GetIntByName(payload, "state") == 0) {
    //    SetState(1);
    //}
    //else {
        //SetState(0);
    //}
}

//�����洢����action��settings��contextΪ��
nlohmann::json allSettings;

void HSDExampleAction::KeyUp(const nlohmann::json& payload) {
    // Log in release and debug builds
    HSDLogger::LogMessage("KeyUp");

    //SetSettings({});
    //ShowOK();
    // Only log in debug builds (C++20-style format strings):
    nlohmann::json settings = payload["settings"];

    if (settings["radio"] == "0") {
        settings["device"] == "output" ? SwitchDefaultAudioPlaybackDevice(StringToWString(settings["select"])) : SwitchDefaultAudioCaptureDevice(StringToWString(settings["select"]));
        settings["current"] = settings["select"];
    }
    else if(settings["current"] == settings["select"]){
        settings["device"] == "output" ? SwitchDefaultAudioPlaybackDevice(StringToWString(settings["select2"])) : SwitchDefaultAudioCaptureDevice(StringToWString(settings["select2"]));
        settings["current"] = settings["select2"];
    }
    else {
        settings["device"] == "output" ? SwitchDefaultAudioPlaybackDevice(StringToWString(settings["select"])) : SwitchDefaultAudioCaptureDevice(StringToWString(settings["select"]));
        settings["current"] = settings["select"];
    }

    if (settings["flag"]) {
        SetState(1);
    }
    else {
        SetState(0);
    }
    if (settings["radio"] == "0") {
        SetState(3);
    }
    settings["flag"] = !settings["flag"];
    SetSettings(settings);
    allSettings[HSDExampleAction::GetContext()] = settings;
    // ����JSON����
    for (auto it = allSettings.begin(); it != allSettings.end(); ++it) {
        if (it.value().contains("current") && it.value()["current"] == settings["current"] && it.value()["radio"] == "0") {
            SetState2(3, it.key());
        }
        else if (it.value()["radio"] == "0") {
            SetState2(2, it.key());
        }
    }
    //HSDLogger::LogMessage("KeyUp end");
}

void HSDExampleAction::WillAppear(const nlohmann::json& payload) {
    HSDLogger::LogMessage("WillAppear");
    try {
        nlohmann::json settings = payload["settings"];
        if (!settings.contains("device")) {
            // ��������ڣ�������Բ����ó�ʼֵ
            settings["device"] = "output";
        }
        std::map<std::wstring, std::wstring> devices = settings["device"] == "output" ? GetAllAudioPlaybackDevices() : GetAllAudioCaptureDevices();
        /*settings["devices"] = devices;*/
        json devicesJson;
        for (const auto& device : devices) {
            json deviceJson;
            deviceJson["deviceId"] = WStringToString(device.second);
            deviceJson["deviceName"] = WStringToString(device.first);
            devicesJson.push_back(deviceJson);
        }
        settings["devices"] = devicesJson;
        if (!settings.contains("select")) {
            // ��������ڣ�������Բ����ó�ʼֵ
            settings["select"] = devicesJson[0]["deviceId"];
        }
        if (!settings.contains("select2")) {
            // ��������ڣ�������Բ����ó�ʼֵ
            settings["select2"] = devicesJson[0]["deviceId"];
        }
        if (!settings.contains("radio")) {
            // ��������ڣ�������Բ����ó�ʼֵ
            settings["radio"] = "0";
        }
        if (!settings.contains("current")) {
            // ��������ڣ�������Բ����ó�ʼֵ
            settings["current"] = devicesJson[0]["deviceId"];
        }
        if (!settings.contains("flag")) {
            // ��������ڣ�������Բ����ó�ʼֵ
            settings["flag"] = true;
        }
        allSettings[HSDExampleAction::GetContext()] = settings;
        if (settings["radio"] == "0") {
            SetState(2);
        }
        else {
            SetState(0);
        }
        SetSettings(settings);
    }
    catch (const std::exception& e)
    {
        std::ofstream logFile("log.txt", std::ios::app); // ��׷��ģʽ���ļ�
  
            logFile << "[ERROR] " << std::string(e.what()) << std::endl;
            logFile.close();

        HSDLogger::LogMessage("WillAppear" + std::string(e.what()));
    }
}

void HSDExampleAction::SendToPlugin(const nlohmann::json& payload) {
    try
    {
        if (!payload.contains("flag2")) {
            nlohmann::json settings = payload;
            std::map<std::wstring, std::wstring> devices = settings["device"] == "output" ? GetAllAudioPlaybackDevices() : GetAllAudioCaptureDevices();

            json devicesJson;
            for (const auto& device : devices) {
                json deviceJson;
                deviceJson["deviceId"] = WStringToString(device.second);
                deviceJson["deviceName"] = WStringToString(device.first);
                devicesJson.push_back(deviceJson);
            }
            settings["devices"] = devicesJson;
            //�л��豸������
            settings["select"] = devicesJson[0]["deviceId"];
            settings["select2"] = devicesJson[0]["deviceId"];
            settings["current"] = devicesJson[0]["deviceId"];
            if (settings["radio"] == "0") {
                SetState(2);
            }
            else {
                SetState(0);
            }
            allSettings[HSDExampleAction::GetContext()] = settings;
            SetSettings(settings);
            //HSDLogger::LogMessage("SendToPlugin: " + settings["radio"]);
        }
        else {
            nlohmann::json settings = payload["settings"];
            if (settings["radio"] == "0") {
                SetState(2);
            }
            else {
                SetState(0);
            }
            allSettings[HSDExampleAction::GetContext()] = settings;
        }
    }
    catch (const std::exception& e)
    {
        HSDLogger::LogMessage("SendToPlugin"+ std::string(e.what()));
    }
}










//------------------------------------------------------------------------mac-----------------------------------------------------------------------------------------
//#include <CoreAudio/CoreAudio.h>
//#include <iostream>
//
////�л�Ĭ����Ƶ����豸
//void SwitchDefaultAudioPlaybackDevice(AudioDeviceID deviceID) {
//    AudioObjectPropertyAddress propertyAddress = {
//        kAudioHardwarePropertyDefaultOutputDevice,  // ����Ҫ���õ�������Ĭ������豸
//        kAudioObjectPropertyScopeGlobal,
//        kAudioObjectPropertyElementMaster
//    };
//
//    OSStatus status = AudioObjectSetPropertyData(kAudioObjectSystemObject, &propertyAddress, 0, nullptr, sizeof(deviceID), &deviceID);
//    if (status != noErr) {
//        std::cerr << "Failed to switch default audio output device: " << status << std::endl;
//    }
//    else {
//        std::cout << "Successfully switched default audio output device." << std::endl;
//    }
//}
//
//
//
////std::string WStringToString(const std::wstring& wstr) {
////    // ʹ�� std::wstring_convert ����ת��
////    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
////    return converter.to_bytes(wstr);
////}
////
////std::wstring StringToWString(const std::string& str) {
////    // ʹ�� std::wstring_convert ����ת��
////    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
////    return converter.from_bytes(str);
////}
//
//void HSDExampleAction::DidReceiveSettings(const nlohmann::json& payload) {
//    HSDLogger::LogMessage("DidReceiveSettings");
//}
//
//void HSDExampleAction::KeyDown(const nlohmann::json& payload) {
//    HSDLogger::LogMessage("KeyDown");
//    //if (NlohmannJSONUtils::GetIntByName(payload, "state") == 0) {
//    //    SetState(1);
//    //}
//    //else {
//        //SetState(0);
//    //}
//}
//
////�����洢����action��settings��contextΪ��
//nlohmann::json allSettings;
//
//void HSDExampleAction::KeyUp(const nlohmann::json& payload) {
//    // Log in release and debug builds
//    HSDLogger::LogMessage("KeyUp");
//
//    //SetSettings({});
//    ShowOK();
//    // Only log in debug builds (C++20-style format strings):
//    nlohmann::json settings = payload["settings"];
//
//    if (settings["radio"] == "0") {
//        settings["device"] == "output" ? SwitchDefaultAudioPlaybackDevice(StringToWString(settings["select"])) : SwitchDefaultAudioCaptureDevice(StringToWString(settings["select"]));
//        settings["current"] = settings["select"];
//    }
//    else if (settings["current"] == settings["select"]) {
//        settings["device"] == "output" ? SwitchDefaultAudioPlaybackDevice(StringToWString(settings["select2"])) : SwitchDefaultAudioCaptureDevice(StringToWString(settings["select2"]));
//        settings["current"] = settings["select2"];
//    }
//    else {
//        settings["device"] == "output" ? SwitchDefaultAudioPlaybackDevice(StringToWString(settings["select"])) : SwitchDefaultAudioCaptureDevice(StringToWString(settings["select"]));
//        settings["current"] = settings["select"];
//    }
//
//    if (settings["flag"]) {
//        SetState(1);
//    }
//    else {
//        SetState(0);
//    }
//    if (settings["radio"] == "0") {
//        SetState(3);
//    }
//    settings["flag"] = !settings["flag"];
//    SetSettings(settings);
//    allSettings[HSDExampleAction::GetContext()] = settings;
//    // ����JSON����
//    for (auto it = allSettings.begin(); it != allSettings.end(); ++it) {
//        if (it.value().contains("current") && it.value()["current"] == settings["current"] && it.value()["radio"] == "0") {
//            SetState2(3, it.key());
//        }
//        else if (it.value()["radio"] == "0") {
//            SetState2(2, it.key());
//        }
//    }
//    //HSDLogger::LogMessage("KeyUp end");
//}
//
//void HSDExampleAction::WillAppear(const nlohmann::json& payload) {
//    nlohmann::json settings = payload["settings"];
//    if (!settings.contains("device")) {
//        // ��������ڣ�������Բ����ó�ʼֵ
//        settings["device"] = "output";
//    }
//    std::map<std::wstring, std::wstring> devices = settings["device"] == "output" ? GetAllAudioPlaybackDevices() : GetAllAudioCaptureDevices();
//    /*settings["devices"] = devices;*/
//    json devicesJson;
//    for (const auto& device : devices) {
//        json deviceJson;
//        deviceJson["deviceId"] = WStringToString(device.second);
//        deviceJson["deviceName"] = WStringToString(device.first);
//        devicesJson.push_back(deviceJson);
//    }
//    settings["devices"] = devicesJson;
//    if (!settings.contains("select")) {
//        // ��������ڣ�������Բ����ó�ʼֵ
//        settings["select"] = devicesJson[0]["deviceId"];
//    }
//    if (!settings.contains("select2")) {
//        // ��������ڣ�������Բ����ó�ʼֵ
//        settings["select2"] = devicesJson[0]["deviceId"];
//    }
//    if (!settings.contains("radio")) {
//        // ��������ڣ�������Բ����ó�ʼֵ
//        settings["radio"] = "0";
//    }
//    if (!settings.contains("current")) {
//        // ��������ڣ�������Բ����ó�ʼֵ
//        settings["current"] = devicesJson[0]["deviceId"];
//    }
//    if (!settings.contains("flag")) {
//        // ��������ڣ�������Բ����ó�ʼֵ
//        settings["flag"] = true;
//    }
//    allSettings[HSDExampleAction::GetContext()] = settings;
//    if (settings["radio"] == "0") {
//        SetState(2);
//    }
//    else {
//        SetState(0);
//    }
//    SetSettings(settings);
//}
//
//void HSDExampleAction::SendToPlugin(const nlohmann::json& payload) {
//    if (!payload.contains("flag2")) {
//        nlohmann::json settings = payload;
//        std::map<std::wstring, std::wstring> devices = settings["device"] == "output" ? GetAllAudioPlaybackDevices() : GetAllAudioCaptureDevices();
//
//        json devicesJson;
//        for (const auto& device : devices) {
//            json deviceJson;
//            deviceJson["deviceId"] = WStringToString(device.second);
//            deviceJson["deviceName"] = WStringToString(device.first);
//            devicesJson.push_back(deviceJson);
//        }
//        settings["devices"] = devicesJson;
//        //�л��豸������
//        settings["select"] = devicesJson[0]["deviceId"];
//        settings["select2"] = devicesJson[0]["deviceId"];
//        settings["current"] = devicesJson[0]["deviceId"];
//        if (settings["radio"] == "0") {
//            SetState(2);
//        }
//        else {
//            SetState(0);
//        }
//        allSettings[HSDExampleAction::GetContext()] = settings;
//        SetSettings(settings);
//        //HSDLogger::LogMessage("SendToPlugin: " + settings["radio"]);
//    }
//    else {
//        nlohmann::json settings = payload["settings"];
//        if (settings["radio"] == "0") {
//            SetState(2);
//        }
//        else {
//            SetState(0);
//        }
//        allSettings[HSDExampleAction::GetContext()] = settings;
//    }
//}