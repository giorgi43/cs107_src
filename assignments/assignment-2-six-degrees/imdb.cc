using namespace std;
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "imdb.h"

const char *const imdb::kActorFileName = "actordata";
const char *const imdb::kMovieFileName = "moviedata";

imdb::imdb(const string& directory)
{
  const string actorFileName = directory + "/" + kActorFileName;
  const string movieFileName = directory + "/" + kMovieFileName;

  actorFile = acquireFileMap(actorFileName, actorInfo);
  movieFile = acquireFileMap(movieFileName, movieInfo);
}

bool imdb::good() const
{
  return !( (actorInfo.fd == -1) ||
    (movieInfo.fd == -1) );
}

// struct that wraps key used in imdb::getCredits and imdb::getCast bsearch function
struct w_key {
    const void* ptr; // pointer to chars (name) or `film` struct
    const void* file; // pointer to movieFile/actorFile
};

// custom comparison function for bsearch in imdb::getCredits method
// key is supposed to be w_key struct because cmp_fn must be standalone function
// and not member function. it doesn's have direct access to actorFile so we 
// pass it using struct. memb is pointer to array element
static int cmp_fn_credits(const void* key, const void* memb) {
    const void* file_ptr = ((w_key*) key)->file;
    const char* key_name = (char*) ((w_key*) key)->ptr;
    int offset = *(int *) memb;
    const char* memb_name = (char*) file_ptr + offset;
    return strcmp(key_name, memb_name);
}

// custom comparison function for bsearch in imdb::getCast method
static int cmp_fn_cast(const void* key, const void* memb) {
    const void* file_ptr = ((w_key*) key)->file;
    const film* key_film = (film*) ((w_key*) key)->ptr;

    int offset = *(int *) memb;
    const void* movie_record = (char*) file_ptr + offset;
    const char* memb_name = (char*) movie_record;
    char delta = *((char*) movie_record + strlen(memb_name) + 1);
    int year = ((int) delta) + 1900;

    const film f = {string(memb_name), year};

    if (*key_film == f) {
        return 0;
    } else if (*key_film < f) {
        return -1;
    } else {
        return 1;
    }
}

bool imdb::getCredits(const string& player, vector<film>& films) const { 
    int n_actors = *(int*) this->actorFile;
    const int* offsets_arr = (int*) ((char*) this->actorFile + sizeof(int));
    w_key key = {player.c_str(), this->actorFile};

    int* offset = (int*) bsearch(&key, offsets_arr, n_actors, sizeof(int), cmp_fn_credits);

    if (offset == NULL) {
        return false;
    }
    // actor found in database. now filling vector
	const void* actor_record = (char*) this->actorFile + *offset;
    int name_len = player.length() + 1; // including '\0'
    int padding = name_len + (name_len%2) + sizeof(short);
    while (padding % 4 != 0) {
        padding++;
    }
    int* movie_offsets = (int*) ((char*) actor_record + padding);
    short num_movies = *(short*) ((char*) actor_record + name_len + (name_len%2));

    for (short i = 0; i < num_movies; i++) {
        const void* movie_record = (char*) this->movieFile + movie_offsets[i];
        const char* movie_name = (char*) movie_record;
        char delta = *((char*) movie_record + strlen(movie_name) + 1);
        int year = ((int) delta) + 1900;
        film f{movie_name, year};
        films.push_back(f);
    }
    return true;
}
bool imdb::getCast(const film& movie, vector<string>& players) const {
    int n_movies = *(int*) this->movieFile;
    const int* offsets_arr = (int*) ((char*) this->movieFile + sizeof(int));
    w_key key = {&movie, this->movieFile};

    int* offset = (int*) bsearch(&key, offsets_arr, n_movies, sizeof(int), cmp_fn_cast);

    if (offset == NULL) {
        return false;
    }
    // movie found in database
	const void* movie_record = (char*) this->movieFile + *offset;
    int name_len = movie.title.length() + 1; // including '\0'
    int padding = name_len + 1; // including one byte for year delta
    while (padding%2 != 0) {
        padding++;
    }
    short num_actors = *((short*) ((char*) movie_record + padding));
    padding += sizeof(short);
    while (padding % 4 != 0) {
        padding++;
    }
    const int* actor_offsets = (int*) ((char*) movie_record + padding);
    for (short i = 0; i < num_actors; i++) {
        const void* actor_record = (char*) this->actorFile + actor_offsets[i];
        const char* actor_name = (char*) actor_record;
        players.push_back(string(actor_name));
    }
    return true;
}

imdb::~imdb()
{
  releaseFileMap(actorInfo);
  releaseFileMap(movieInfo);
}

// ignore everything below... it's all UNIXy stuff in place to make a file look like
// an array of bytes in RAM.. 
const void *imdb::acquireFileMap(const string& fileName, struct fileInfo& info)
{
  struct stat stats;
  stat(fileName.c_str(), &stats);
  info.fileSize = stats.st_size;
  info.fd = open(fileName.c_str(), O_RDONLY);
  return info.fileMap = mmap(0, info.fileSize, PROT_READ, MAP_SHARED, info.fd, 0);
}

void imdb::releaseFileMap(struct fileInfo& info)
{
  if (info.fileMap != NULL) munmap((char *) info.fileMap, info.fileSize);
  if (info.fd != -1) close(info.fd);
}
