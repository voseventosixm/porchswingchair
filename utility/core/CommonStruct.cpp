#include "CommonStruct.h"

string GetIdentifyDataString(const U8* buffer)
{
    U8 revBuffer[512];
    for (int i = 0; i < 512; i+=2) {
        revBuffer[i+0] = buffer[i+1];
        revBuffer[i+1] = buffer[i+0];
    }

    stringstream sstr;
    const IdentifyDeviceData& data = *((IdentifyDeviceData*) revBuffer);

    sstr << "ModelNumber: " << string((char*)data.ModelNumber, 40) << endl;
    sstr << "SerialNumber: " << string((char*)data.SerialNumber, 20) << endl;

    if(0) {
        sstr << "CommandSet supported (w80)" << endl;
        if (data.MajorRevision & 0x0400) sstr << "Support ACS-3" << endl;
        if (data.MajorRevision & 0x0200) sstr << "Support ACS-2" << endl;
        if (data.MajorRevision & 0x0100) sstr << "Support ATA8-ACS" << endl;
        if (data.MajorRevision & 0x0080) sstr << "Support ATA/ATAPI-7" << endl;
        if (data.MajorRevision & 0x0040) sstr << "Support ATA/ATAPI-6" << endl;
        if (data.MajorRevision & 0x0020) sstr << "Support ATA/ATAPI-5" << endl;
    }

    if(0) {
        #define TEST_COND(name, text) \
            sstr << "Support " << setw(32) << text << ":" << (data.CommandSetSupport.name ? "Yes" : "No") << endl

        sstr << "FeatureSet supported (w82)" << endl;
        TEST_COND(SmartCommands, "SMART Feature");
        TEST_COND(SecurityMode, "Security Feature");
        TEST_COND(PowerManagement, "Power Management Feature");
        TEST_COND(WriteCache, "Volatile WriteCache");
        TEST_COND(LookAhead, "Read LookAhead");
        TEST_COND(WriteBuffer, "WriteBuffer command");
        TEST_COND(ReadBuffer, "ReadBuffer command");
        TEST_COND(Nop, "NOP command");
    }

    return sstr.str();
}

string GetInquiryDataString(const U8* buffer)
{
    stringstream sstr;
    const InquiryData& data = *((InquiryData*) buffer);

    sstr << "VendorId: " << string((char*)data.VendorId, 8) << endl;
    sstr << "ProductId: " << string((char*)data.ProductId, 16) << endl;
    sstr << "VendorSpecific: " << string((char*)data.VendorSpecific, 20) << endl;

    return sstr.str();
}
