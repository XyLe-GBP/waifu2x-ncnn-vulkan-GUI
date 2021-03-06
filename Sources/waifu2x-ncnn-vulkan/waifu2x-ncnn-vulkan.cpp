﻿
// waifu2x-ncnn-vulkan.cpp : アプリケーションのクラス動作を定義します。
//

#include "pch.h"
#include "framework.h"
#include "waifu2x-ncnn-vulkan.h"
#include "waifu2x-ncnn-vulkanDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Cwaifu2xncnnvulkanApp

BEGIN_MESSAGE_MAP(Cwaifu2xncnnvulkanApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// Cwaifu2xncnnvulkanApp の構築

Cwaifu2xncnnvulkanApp::Cwaifu2xncnnvulkanApp()
{
	// 再起動マネージャーをサポートします
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: この位置に構築用コードを追加してください。
	// ここに InitInstance 中の重要な初期化処理をすべて記述してください。
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_DELAY_FREE_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(275);
	g_hMutex = NULL;
}


// 唯一の Cwaifu2xncnnvulkanApp オブジェクト

Cwaifu2xncnnvulkanApp theApp;

// Cwaifu2xncnnvulkanApp の初期化

BOOL Cwaifu2xncnnvulkanApp::InitInstance()
{
	// アプリケーション マニフェストが visual スタイルを有効にするために、
	// ComCtl32.dll Version 6 以降の使用を指定する場合は、
	// Windows XP に InitCommonControlsEx() が必要です。さもなければ、ウィンドウ作成はすべて失敗します。
	INITCOMMONCONTROLSEX InitCtrls{};
	InitCtrls.dwSize = sizeof(InitCtrls);
	// アプリケーションで使用するすべてのコモン コントロール クラスを含めるには、
	// これを設定します。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	bool bWaitTerminate = false;
	DWORD dwWaitProcessId;
	for (int i = 1; i < __argc; ++i) {

		if (!(__wargv[i][0] == _T('/') || __wargv[i][0] == _T('-'))) {
			continue;
		}
		if (_tcsicmp(&__wargv[i][1], _T("waitterminate")) == 0) {

			if (++i >= __argc) {
				continue;
			}
			dwWaitProcessId = static_cast<DWORD>(_ttoi(__wargv[i]));
			if (dwWaitProcessId != 0) {
				bWaitTerminate = true;
			}
		}
	}
	if (bWaitTerminate) {
		HANDLE hWaitProcess = ::OpenProcess(SYNCHRONIZE, FALSE, dwWaitProcessId);
		if (hWaitProcess != NULL) {
			// 10秒待っても前のプロセスが終了しない場合は、アプリケーションを終了します。
			if (::WaitForSingleObject(hWaitProcess, 10 * 1000) == WAIT_TIMEOUT) {
				return FALSE;
			}
		}
	}

	SECURITY_DESCRIPTOR sd{};
	if (0 == ::InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION)) {
		// エラー
		return FALSE;
	}

	if (0 == ::SetSecurityDescriptorDacl(&sd, TRUE, 0, FALSE)) {
		// エラー
		return FALSE;
	}

	InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(&sd, TRUE, 0, FALSE);
	SECURITY_ATTRIBUTES secAttribute{};
	secAttribute.nLength = sizeof(secAttribute);
	secAttribute.lpSecurityDescriptor = &sd;
	secAttribute.bInheritHandle = TRUE;

	g_hMutex = CreateMutex(&secAttribute, FALSE, _T("waifu2xnvgui.mutex.exe"));
	if (g_hMutex == NULL) {
		return FALSE;
	}

	if (::GetLastError() == ERROR_ALREADY_EXISTS)
	{
		SetLastError(NO_ERROR);
		if (g_hMutex) {
			CloseHandle(g_hMutex);
		}
		MessageBox(NULL,_T("Double launching of applications is not allowed."), _T("Error"), MB_ICONERROR | MB_OK);
		return FALSE;
	}

	AfxEnableControlContainer();

	// ダイアログにシェル ツリー ビューまたはシェル リスト ビュー コントロールが
	// 含まれている場合にシェル マネージャーを作成します。
	CShellManager *pShellManager = new CShellManager;

	// MFC コントロールでテーマを有効にするために、"Windows ネイティブ" のビジュアル マネージャーをアクティブ化
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// 標準初期化
	// これらの機能を使わずに最終的な実行可能ファイルの
	// サイズを縮小したい場合は、以下から不要な初期化
	// ルーチンを削除してください。
	// 設定が格納されているレジストリ キーを変更します。
	// TODO: 会社名または組織名などの適切な文字列に
	// この文字列を変更してください。
	SetRegistryKey(_T("アプリケーション ウィザードで生成されたローカル アプリケーション"));

	Cwaifu2xncnnvulkanDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: ダイアログが <OK> で消された時のコードを
		//  記述してください。
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: ダイアログが <キャンセル> で消された時のコードを
		//  記述してください。
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "警告: ダイアログの作成に失敗しました。アプリケーションは予期せずに終了します。\n");
		TRACE(traceAppMsg, 0, "警告: ダイアログで MFC コントロールを使用している場合、#define _AFX_NO_MFC_CONTROLS_IN_DIALOGS を指定できません。\n");
	}
	else if (nResponse == 2)
	{

	}

	// 上で作成されたシェル マネージャーを削除します。
	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// ダイアログは閉じられました。アプリケーションのメッセージ ポンプを開始しないで
	//  アプリケーションを終了するために FALSE を返してください。
	return FALSE;
}

BOOL Cwaifu2xncnnvulkanApp::ExitInstance()
{
	if (g_hMutex)
	{
		ReleaseMutex(g_hMutex);
		CloseHandle(g_hMutex);
		g_hMutex = NULL;
	}

	return CWinApp::ExitInstance();
}