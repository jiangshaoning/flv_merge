//stamp:0750dcf46c508554
/*<------------------------------------------------------------------------------------------------->*/
/*该文件由uiresbuilder生成，请不要手动修改*/
/*<------------------------------------------------------------------------------------------------->*/
#pragma once
#include <res.mgr/snamedvalue.h>
#define ROBJ_IN_CPP \
namespace SOUI \
{\
    const _R R;\
    const _UIRES UIRES;\
}
namespace SOUI
{
	class _UIRES{
		public:
		class _UIDEF{
			public:
			_UIDEF(){
				XML_INIT = _T("UIDEF:XML_INIT");
			}
			const TCHAR * XML_INIT;
		}UIDEF;
		class _LAYOUT{
			public:
			_LAYOUT(){
				XML_MAINWND = _T("LAYOUT:XML_MAINWND");
			}
			const TCHAR * XML_MAINWND;
		}LAYOUT;
		class _values{
			public:
			_values(){
				string = _T("values:string");
				color = _T("values:color");
				skin = _T("values:skin");
			}
			const TCHAR * string;
			const TCHAR * color;
			const TCHAR * skin;
		}values;
		class _IMG{
			public:
			_IMG(){
			}
		}IMG;
		class _ICON{
			public:
			_ICON(){
				ICON_LOGO = _T("ICON:ICON_LOGO");
			}
			const TCHAR * ICON_LOGO;
		}ICON;
		class _PNG{
			public:
			_PNG(){
				ID_PNG_BG = _T("PNG:ID_PNG_BG");
				png_tab_left = _T("PNG:png_tab_left");
			}
			const TCHAR * ID_PNG_BG;
			const TCHAR * png_tab_left;
		}PNG;
		class _translator{
			public:
			_translator(){
				lang_cn = _T("translator:lang_cn");
				lang_eg = _T("translator:lang_eg");
			}
			const TCHAR * lang_cn;
			const TCHAR * lang_eg;
		}translator;
	};
	const SNamedID::NAMEDVALUE namedXmlID[]={
		{L"_name_start",65535},
		{L"add_flv",65541},
		{L"btn_cancel",65548},
		{L"btn_close",65536},
		{L"btn_exit",65549},
		{L"btn_generate",65547},
		{L"btn_max",65537},
		{L"btn_min",65539},
		{L"btn_restore",65538},
		{L"cbx_language",65550},
		{L"choose_path",65545},
		{L"clear_list",65543},
		{L"flv_list",65540},
		{L"merge_progress",65546},
		{L"outfile_path",65544},
		{L"remove_flv",65542}		};
	class _R{
	public:
		class _name{
		public:
		_name(){
			_name_start = namedXmlID[0].strName;
			add_flv = namedXmlID[1].strName;
			btn_cancel = namedXmlID[2].strName;
			btn_close = namedXmlID[3].strName;
			btn_exit = namedXmlID[4].strName;
			btn_generate = namedXmlID[5].strName;
			btn_max = namedXmlID[6].strName;
			btn_min = namedXmlID[7].strName;
			btn_restore = namedXmlID[8].strName;
			cbx_language = namedXmlID[9].strName;
			choose_path = namedXmlID[10].strName;
			clear_list = namedXmlID[11].strName;
			flv_list = namedXmlID[12].strName;
			merge_progress = namedXmlID[13].strName;
			outfile_path = namedXmlID[14].strName;
			remove_flv = namedXmlID[15].strName;
		}
		 const wchar_t * _name_start;
		 const wchar_t * add_flv;
		 const wchar_t * btn_cancel;
		 const wchar_t * btn_close;
		 const wchar_t * btn_exit;
		 const wchar_t * btn_generate;
		 const wchar_t * btn_max;
		 const wchar_t * btn_min;
		 const wchar_t * btn_restore;
		 const wchar_t * cbx_language;
		 const wchar_t * choose_path;
		 const wchar_t * clear_list;
		 const wchar_t * flv_list;
		 const wchar_t * merge_progress;
		 const wchar_t * outfile_path;
		 const wchar_t * remove_flv;
		}name;

		class _id{
		public:
		const static int _name_start	=	65535;
		const static int add_flv	=	65541;
		const static int btn_cancel	=	65548;
		const static int btn_close	=	65536;
		const static int btn_exit	=	65549;
		const static int btn_generate	=	65547;
		const static int btn_max	=	65537;
		const static int btn_min	=	65539;
		const static int btn_restore	=	65538;
		const static int cbx_language	=	65550;
		const static int choose_path	=	65545;
		const static int clear_list	=	65543;
		const static int flv_list	=	65540;
		const static int merge_progress	=	65546;
		const static int outfile_path	=	65544;
		const static int remove_flv	=	65542;
		}id;

		class _string{
		public:
		const static int title	=	0;
		const static int ver	=	1;
		}string;

		class _color{
		public:
		const static int blue	=	0;
		const static int gray	=	1;
		const static int green	=	2;
		const static int red	=	3;
		const static int white	=	4;
		}color;

	};

#ifdef R_IN_CPP
	 extern const _R R;
	 extern const _UIRES UIRES;
#else
	 extern const __declspec(selectany) _R & R = _R();
	 extern const __declspec(selectany) _UIRES & UIRES = _UIRES();
#endif//R_IN_CPP
}
