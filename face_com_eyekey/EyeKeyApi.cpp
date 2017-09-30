#include "StdAfx.h"
#include "EyeKeyApi.h"

EyeKeyApi::EyeKeyApi(CString strAppID, CString strAppKey)
{
    m_strAppID_Key = _T("app_id=") + strAppID + _T("&app_key=") + strAppKey;
}

EyeKeyApi::~EyeKeyApi(void)
{
}

BOOL EyeKeyApi::Check(CString strImg, CString& strRes)
{
    //base64±àÂëÍ¼Æ¬
    CString strBase64Img = GetBase64Image(strImg.GetBuffer(0)).c_str();
    CString strParams = m_strAppID_Key;

    if (strBase64Img != "")
    {
        strParams += "&img=";
        strParams += strBase64Img;
    }else{
        printf("GetBase64Img error\n");
        return FALSE;
    }

    m_req.HttpPost(_T("http://api.eyekey.com/face/Check/checking"),strParams, strRes);

    return TRUE;
}

BOOL EyeKeyApi::ComPareFace(CString strFaceID1, CString strFaceID2, CString& strRes)
{
    CString strUrl = _T("http://api.eyekey.com/face/Match/match_compare?") + m_strAppID_Key + 
        _T("&face_id1=") + strFaceID1 + _T("&face_id2=") + strFaceID2;

    m_req.HttpGet(strUrl,_T(""),strRes);
    
    return TRUE;
}
BOOL EyeKeyApi::CheckMark(CString strFaceID, CString& strRes)
{
	CString strUrl = _T("http://api.eyekey.com/face/Check/check_mark?") + m_strAppID_Key +
		_T("&face_id=") + strFaceID ;

	m_req.HttpGet(strUrl, _T(""), strRes);

	return TRUE;
}
BOOL EyeKeyApi::CreatePeople(CString strPeopleName, CString strFaceID, CString& strRes)
{
    CString strUrl = _T("http://api.eyekey.com/People/people_create?") + m_strAppID_Key + 
        _T("&people_name=") + strPeopleName + _T("&face_id=") + strFaceID;

    m_req.HttpGet(strUrl,_T(""),strRes);

    return TRUE;
}

BOOL EyeKeyApi::DeletePeople(CString strPeopleName, CString& strRes)
{
    CString strUrl = _T("http://api.eyekey.com/People/people_delete?") + m_strAppID_Key + 
        _T("&people_name=") + strPeopleName;

    m_req.HttpGet(strUrl,_T(""),strRes);

    return TRUE;
}


BOOL EyeKeyApi::GetPeople(CString strPeopleName, CString& strRes)
{
    CString strUrl = _T("http://api.eyekey.com/People/people_get?") + m_strAppID_Key + 
        _T("&people_name=") + strPeopleName;

    m_req.HttpGet(strUrl,_T(""),strRes);

    return TRUE;
}

BOOL EyeKeyApi::RenamePeople(CString strPeopleOldName, CString strPeopleNewName, CString& strRes)
{
    CString strUrl = _T("http://api.eyekey.com/People/people_set?") + m_strAppID_Key + 
        _T("&people_name=") + strPeopleNewName + _T("name=") + strPeopleOldName;

    m_req.HttpGet(strUrl,_T(""),strRes);

    return TRUE;
}