/*
Created by Sergey Trubin at 2023-08-31
*/

#ifndef FILE_LIST_H
#define FILE_LIST_H 1

#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif
    
#define _MAX_SIZE 8U
    
struct SkimpHeader{
      char mark[4] = {'S','A', 'T', 'S'};
      unsigned begin:30;
      unsigned max_size:30;
      
      SkimpHeader()
      {
          begin = 0;
          max_size = _MAX_SIZE;
      }
};


class FileStat: public stat {
//     using st_size;
public:
	//struct stat fileStat;
    SkimpHeader sh;
	//std::string fileText; //content of file (ram)
	std::fstream content; //content of file (phys)
	
	
	FILE *p_file;

	FileStat() : p_file (nullptr) {
	   st_uid = 0;
	   st_gid = 0;


       st_dev = 0;         /* ID устройства с файлом */
       st_ino = 0;         /* номер иноды */
       st_mode = S_IFREG | 0444 ;        /* тип файла и режим доступа */
       st_nlink = 1;         /* количество жёстких ссылок */
       st_uid   = 0;         /* идентификатор пользователя-владельца */
       st_gid   = 0;         /* идентификатор группы-владельца */
       st_rdev  = 0;         /* идентификатор устройства
                                            (для специального файла) */
       st_size    = 0;        /* общий размер в байтах */

       st_blksize = 0;     /* размер блока ввода-вывода файловой системы */
       st_blocks  = 0;      /* количество выделенных 512Б блоков */
       
       

               /* Начиная с Linux 2.6, ядро поддерживает точность до
                  наносекунд в следующих полям меток времени.
                  Подробней о версиях до Linux 2.6, смотрите ЗАМЕЧАНИЯ. */

       //st_atim = 0;  /* время последнего доступа */
       //st_mtim = 0;  /* время последнего изменения */
       //st_ctim = 0;  /* время последней смены состояния */

      //SYSLOG("%s %s",__func__, "DEFAULT");
	}

	FileStat(mode_t mode): FileStat() {
		st_mode = mode;
	}

	FileStat(const stat& st): stat(st), p_file (nullptr)/*,fileText()*/ {}

	FileStat(const FileStat& fl) :stat(fl), p_file (nullptr)/*, fileText(fl.fileText)*/ {}
 
 	FileStat(const std::string& str):FileStat()
    {
    //  fileText=str;
     }

	FileStat & operator =(const FileStat& fl)
	{
        *this = fl;
       // fileText=fl.fileText;
        return *this;
	}

	~FileStat() {
        if (p_file) {
            fclose(p_file);
        }
	}



};

// struct Skimpfs_t : public FileStat {
//     
//     
//     Skimpfs_t() : begin(0), FileStat() {
//         
//     }
//     
//  //  Skimpfs_t(const Skimpfs_t& s) : FileStat(s), begin(s.begin) {}
//     Skimpfs_t(const stat& st): FileStat(st) {}
//     
//     Skimpfs_t(mode_t mode) : begin(0), FileStat(mode) {
//         
//     }
// 
// };

#ifdef __cplusplus
}
#endif

#endif //FILE_LIST_H

