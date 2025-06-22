#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_TITLE 100
#define MAX_AUTHOR 100
#define MAX_GENRE 50
#define MAX_MAGAZINE 50
#define MAX_PUBLISHER 50
#define MAX_VOLUMES 100
#define ISBN_SIZE 20

// Estrutura para armazenar dados do mangá
typedef struct {
    char isbn[ISBN_SIZE];
    char title[MAX_TITLE];
    char author[MAX_AUTHOR];
    int start_year;
    int end_year;
    char genre[MAX_GENRE];
    char magazine[MAX_MAGAZINE];
    char publisher[MAX_PUBLISHER];
    int edition_year;
    int total_volumes;
    int acquired_volumes;
    int volumes_list[MAX_VOLUMES];
    int deleted; // 0 = ativo, 1 = deletado
} Manga;

// Estrutura para índice primário (ISBN)
typedef struct {
    char isbn[ISBN_SIZE];
    long offset;
} PrimaryIndex;

// Estrutura para índice secundário (Título)
typedef struct {
    char title[MAX_TITLE];
    char isbn[ISBN_SIZE];
} SecondaryIndex;

// Variáveis globais para os índices
PrimaryIndex *primary_indices = NULL;
SecondaryIndex *secondary_indices = NULL;
int primary_count = 0;
int secondary_count = 0;

// Função para remover espaços extras e converter para lowercase
void normalize_string(char *str) {
    char temp[MAX_TITLE];
    int i = 0, j = 0;
    
    // Remover espaços do início
    while (str[i] && isspace(str[i])) {
        i++;
    }
    
    // Copiar caracteres convertendo para lowercase e removendo espaços extras
    while (str[i]) {
        if (isspace(str[i])) {
            // Adicionar apenas um espaço se não for o último caractere
            if (j > 0 && temp[j-1] != ' ') {
                temp[j++] = ' ';
            }
        } else {
            temp[j++] = tolower(str[i]);
        }
        i++;
    }
    
    // Remover espaço do final
    if (j > 0 && temp[j-1] == ' ') {
        j--;
    }
    
    temp[j] = '\0';
    strcpy(str, temp);
}

// Função para verificar se uma string contém outra (busca parcial)
int contains_substring(const char *haystack, const char *needle) {
    char normalized_haystack[MAX_TITLE], normalized_needle[MAX_TITLE];
    
    strcpy(normalized_haystack, haystack);
    strcpy(normalized_needle, needle);
    
    normalize_string(normalized_haystack);
    normalize_string(normalized_needle);
    
    return strstr(normalized_haystack, normalized_needle) != NULL;
}

// Função para comparar títulos de forma mais flexível
int compare_titles(const char *title1, const char *title2) {
    char normalized1[MAX_TITLE], normalized2[MAX_TITLE];
    
    strcpy(normalized1, title1);
    strcpy(normalized2, title2);
    
    normalize_string(normalized1);
    normalize_string(normalized2);
    
    return strcmp(normalized1, normalized2);
}

// Função para comparar índices primários (por ISBN)
int compare_primary(const void *a, const void *b) {
    return strcmp(((PrimaryIndex*)a)->isbn, ((PrimaryIndex*)b)->isbn);
}

// Função para comparar índices secundários (por título)
int compare_secondary(const void *a, const void *b) {
    return compare_titles(((SecondaryIndex*)a)->title, ((SecondaryIndex*)b)->title);
}

// Carregar índices primários do arquivo
void load_primary_indices() {
    FILE *file = fopen("primary_index.dat", "rb");
    if (!file) {
        primary_count = 0;
        return;
    }
    
    fread(&primary_count, sizeof(int), 1, file);
    if (primary_count > 0) {
        primary_indices = malloc(primary_count * sizeof(PrimaryIndex));
        fread(primary_indices, sizeof(PrimaryIndex), primary_count, file);
    }
    fclose(file);
}

// Salvar índices primários no arquivo
void save_primary_indices() {
    FILE *file = fopen("primary_index.dat", "wb");
    if (!file) {
        printf("Erro ao salvar índices primários!\n");
        return;
    }
    
    fwrite(&primary_count, sizeof(int), 1, file);
    if (primary_count > 0) {
        fwrite(primary_indices, sizeof(PrimaryIndex), primary_count, file);
    }
    fclose(file);
}

// Carregar índices secundários do arquivo
void load_secondary_indices() {
    FILE *file = fopen("secondary_index.dat", "rb");
    if (!file) {
        secondary_count = 0;
        return;
    }
    
    fread(&secondary_count, sizeof(int), 1, file);
    if (secondary_count > 0) {
        secondary_indices = malloc(secondary_count * sizeof(SecondaryIndex));
        fread(secondary_indices, sizeof(SecondaryIndex), secondary_count, file);
    }
    fclose(file);
}

// Salvar índices secundários no arquivo
void save_secondary_indices() {
    FILE *file = fopen("secondary_index.dat", "wb");
    if (!file) {
        printf("Erro ao salvar índices secundários!\n");
        return;
    }
    
    fwrite(&secondary_count, sizeof(int), 1, file);
    if (secondary_count > 0) {
        fwrite(secondary_indices, sizeof(SecondaryIndex), secondary_count, file);
    }
    fclose(file);
}

// Buscar manga por ISBN no índice primário
long find_manga_by_isbn(const char *isbn) {
    PrimaryIndex key;
    strcpy(key.isbn, isbn);
    
    PrimaryIndex *result = bsearch(&key, primary_indices, primary_count, 
                                   sizeof(PrimaryIndex), compare_primary);
    
    return result ? result->offset : -1;
}

// Buscar ISBN por título no índice secundário (busca exata e parcial)
char* find_isbn_by_title(const char *title) {
    char normalized_search[MAX_TITLE];
    strcpy(normalized_search, title);
    normalize_string(normalized_search);
    
    // Primeiro: busca exata
    for (int i = 0; i < secondary_count; i++) {
        char normalized_title[MAX_TITLE];
        strcpy(normalized_title, secondary_indices[i].title);
        normalize_string(normalized_title);
        
        if (strcmp(normalized_search, normalized_title) == 0) {
            return secondary_indices[i].isbn;
        }
    }
    
    // Segundo: busca parcial (substring)
    for (int i = 0; i < secondary_count; i++) {
        if (contains_substring(secondary_indices[i].title, title)) {
            return secondary_indices[i].isbn;
        }
    }
    
    return NULL;
}

// Buscar múltiplos ISBNs por título parcial
void find_multiple_by_partial_title(const char *search_term, char results[][ISBN_SIZE], int *count, int max_results) {
    *count = 0;
    
    for (int i = 0; i < secondary_count && *count < max_results; i++) {
        if (contains_substring(secondary_indices[i].title, search_term)) {
            strcpy(results[*count], secondary_indices[i].isbn);
            (*count)++;
        }
    }
}

// Adicionar índice primário
void add_primary_index(const char *isbn, long offset) {
    primary_indices = realloc(primary_indices, (primary_count + 1) * sizeof(PrimaryIndex));
    strcpy(primary_indices[primary_count].isbn, isbn);
    primary_indices[primary_count].offset = offset;
    primary_count++;
    
    qsort(primary_indices, primary_count, sizeof(PrimaryIndex), compare_primary);
}

// Adicionar índice secundário
void add_secondary_index(const char *title, const char *isbn) {
    secondary_indices = realloc(secondary_indices, (secondary_count + 1) * sizeof(SecondaryIndex));
    strcpy(secondary_indices[secondary_count].title, title);
    strcpy(secondary_indices[secondary_count].isbn, isbn);
    secondary_count++;
    
    qsort(secondary_indices, secondary_count, sizeof(SecondaryIndex), compare_secondary);
}

// Remover índice primário
void remove_primary_index(const char *isbn) {
    for (int i = 0; i < primary_count; i++) {
        if (strcmp(primary_indices[i].isbn, isbn) == 0) {
            for (int j = i; j < primary_count - 1; j++) {
                primary_indices[j] = primary_indices[j + 1];
            }
            primary_count--;
            primary_indices = realloc(primary_indices, primary_count * sizeof(PrimaryIndex));
            break;
        }
    }
}

// Remover índice secundário
void remove_secondary_index(const char *title) {
    for (int i = 0; i < secondary_count; i++) {
        if (strcmp(secondary_indices[i].title, title) == 0) {
            for (int j = i; j < secondary_count - 1; j++) {
                secondary_indices[j] = secondary_indices[j + 1];
            }
            secondary_count--;
            secondary_indices = realloc(secondary_indices, secondary_count * sizeof(SecondaryIndex));
            break;
        }
    }
}

// Função para debug - mostra todos os títulos indexados
void debug_titles() {
    printf("\n=== DEBUG: TÍTULOS INDEXADOS ===\n");
    for (int i = 0; i < secondary_count; i++) {
        char normalized[MAX_TITLE];
        strcpy(normalized, secondary_indices[i].title);
        normalize_string(normalized);
        printf("%d. Original: '%s'\n", i+1, secondary_indices[i].title);
        printf("   Normalizado: '%s'\n", normalized);
        printf("   ISBN: %s\n\n", secondary_indices[i].isbn);
    }
}

// Exibir dados de um mangá
void display_manga(const Manga *manga) {
    printf("\n=== DADOS DO MANGÁ ===\n");
    printf("ISBN: %s\n", manga->isbn);
    printf("Título: %s\n", manga->title);
    printf("Autor(es): %s\n", manga->author);
    printf("Ano de início: %d\n", manga->start_year);
    if (manga->end_year == -1) {
        printf("Ano de fim: Em publicação\n");
    } else {
        printf("Ano de fim: %d\n", manga->end_year);
    }
    printf("Gênero: %s\n", manga->genre);
    printf("Revista: %s\n", manga->magazine);
    printf("Editora: %s\n", manga->publisher);
    printf("Ano da edição: %d\n", manga->edition_year);
    printf("Total de volumes: %d\n", manga->total_volumes);
    printf("Volumes adquiridos: %d\n", manga->acquired_volumes);
    printf("Lista de volumes: ");
    for (int i = 0; i < manga->acquired_volumes; i++) {
        printf("%d ", manga->volumes_list[i]);
    }
    printf("\n");
}

// Criar novo registro de mangá
void create_manga() {
    Manga manga;
    
    printf("\n=== CRIAR NOVO MANGÁ ===\n");
    
    printf("ISBN: ");
    scanf("%s", manga.isbn);
    
    // Verificar se ISBN já existe
    if (find_manga_by_isbn(manga.isbn) != -1) {
        printf("Erro: ISBN já existe!\n");
        return;
    }
    
    printf("Título: ");
    getchar(); // Limpar buffer
    fgets(manga.title, MAX_TITLE, stdin);
    manga.title[strcspn(manga.title, "\n")] = 0; // Remover \n
    
    printf("Autor(es): ");
    fgets(manga.author, MAX_AUTHOR, stdin);
    manga.author[strcspn(manga.author, "\n")] = 0;
    
    printf("Ano de início: ");
    scanf("%d", &manga.start_year);
    
    printf("Ano de fim (-1 se ainda em publicação): ");
    scanf("%d", &manga.end_year);
    
    printf("Gênero: ");
    getchar();
    fgets(manga.genre, MAX_GENRE, stdin);
    manga.genre[strcspn(manga.genre, "\n")] = 0;
    
    printf("Revista: ");
    fgets(manga.magazine, MAX_MAGAZINE, stdin);
    manga.magazine[strcspn(manga.magazine, "\n")] = 0;
    
    printf("Editora: ");
    fgets(manga.publisher, MAX_PUBLISHER, stdin);
    manga.publisher[strcspn(manga.publisher, "\n")] = 0;
    
    printf("Ano da edição: ");
    scanf("%d", &manga.edition_year);
    
    printf("Quantidade total de volumes: ");
    scanf("%d", &manga.total_volumes);
    
    printf("Quantidade de volumes adquiridos: ");
    scanf("%d", &manga.acquired_volumes);
    
    printf("Digite os volumes adquiridos (separados por espaço): ");
    for (int i = 0; i < manga.acquired_volumes; i++) {
        scanf("%d", &manga.volumes_list[i]);
    }
    
    manga.deleted = 0;
    
    // Salvar no arquivo de dados
    FILE *file = fopen("mangas.dat", "ab");
    if (!file) {
        printf("Erro ao abrir arquivo de dados!\n");
        return;
    }
    
    long offset = ftell(file);
    fwrite(&manga, sizeof(Manga), 1, file);
    fclose(file);
    
    // Atualizar índices
    add_primary_index(manga.isbn, offset);
    add_secondary_index(manga.title, manga.isbn);
    
    save_primary_indices();
    save_secondary_indices();
    
    printf("Mangá criado com sucesso!\n");
}

// Ler mangá por ISBN ou título
void read_manga() {
    char search[MAX_TITLE];
    long offset;
    char *isbn;
    
    printf("\n=== BUSCAR MANGÁ ===\n");
    printf("Digite o ISBN ou título (pode ser parcial): ");
    getchar();
    fgets(search, MAX_TITLE, stdin);
    search[strcspn(search, "\n")] = 0;
    
    // Tentar buscar por ISBN primeiro
    offset = find_manga_by_isbn(search);
    
    // Se não encontrar, buscar por título
    if (offset == -1) {
        isbn = find_isbn_by_title(search);
        if (isbn) {
            offset = find_manga_by_isbn(isbn);
        }
    }
    
    if (offset == -1) {
        // Buscar múltiplos resultados para busca parcial
        char results[10][ISBN_SIZE];
        int count;
        find_multiple_by_partial_title(search, results, &count, 10);
        
        if (count == 0) {
            printf("Mangá não encontrado!\n");
            return;
        } else if (count == 1) {
            offset = find_manga_by_isbn(results[0]);
        } else {
            printf("\nEncontrados %d mangás:\n", count);
            for (int i = 0; i < count; i++) {
                // Buscar e exibir título para cada resultado
                for (int j = 0; j < secondary_count; j++) {
                    if (strcmp(secondary_indices[j].isbn, results[i]) == 0) {
                        printf("%d. %s (%s)\n", i+1, secondary_indices[j].title, results[i]);
                        break;
                    }
                }
            }
            
            int choice;
            printf("Escolha um mangá (1-%d): ", count);
            scanf("%d", &choice);
            
            if (choice < 1 || choice > count) {
                printf("Opção inválida!\n");
                return;
            }
            
            offset = find_manga_by_isbn(results[choice-1]);
        }
    }
    
    if (offset == -1) {
        printf("Erro ao localizar mangá!\n");
        return;
    }
    
    // Ler do arquivo
    FILE *file = fopen("mangas.dat", "rb");
    if (!file) {
        printf("Erro ao abrir arquivo de dados!\n");
        return;
    }
    
    Manga manga;
    fseek(file, offset, SEEK_SET);
    fread(&manga, sizeof(Manga), 1, file);
    fclose(file);
    
    if (manga.deleted) {
        printf("Mangá foi deletado!\n");
        return;
    }
    
    display_manga(&manga);
}

// Atualizar mangá
void update_manga() {
    char search[MAX_TITLE];
    long offset;
    char *isbn;
    
    printf("\n=== ATUALIZAR MANGÁ ===\n");
    printf("Digite o ISBN ou título do mangá a ser atualizado: ");
    getchar();
    fgets(search, MAX_TITLE, stdin);
    search[strcspn(search, "\n")] = 0;
    
    // Buscar mangá
    offset = find_manga_by_isbn(search);
    if (offset == -1) {
        isbn = find_isbn_by_title(search);
        if (isbn) {
            offset = find_manga_by_isbn(isbn);
        }
    }
    
    if (offset == -1) {
        printf("Mangá não encontrado!\n");
        return;
    }
    
    // Ler mangá atual
    FILE *file = fopen("mangas.dat", "r+b");
    if (!file) {
        printf("Erro ao abrir arquivo de dados!\n");
        return;
    }
    
    Manga manga;
    fseek(file, offset, SEEK_SET);
    fread(&manga, sizeof(Manga), 1, file);
    
    if (manga.deleted) {
        printf("Mangá foi deletado!\n");
        fclose(file);
        return;
    }
    
    char old_title[MAX_TITLE];
    strcpy(old_title, manga.title);
    
    // Atualizar campos
    printf("Título atual: %s\n", manga.title);
    printf("Novo título (Enter para manter): ");
    char new_title[MAX_TITLE];
    fgets(new_title, MAX_TITLE, stdin);
    if (strlen(new_title) > 1) {
        new_title[strcspn(new_title, "\n")] = 0;
        strcpy(manga.title, new_title);
    }
    
    printf("Volumes adquiridos atual: %d\n", manga.acquired_volumes);
    printf("Novos volumes adquiridos (-1 para manter): ");
    int new_acquired;
    scanf("%d", &new_acquired);
    if (new_acquired != -1) {
        manga.acquired_volumes = new_acquired;
        printf("Digite os novos volumes adquiridos: ");
        for (int i = 0; i < manga.acquired_volumes; i++) {
            scanf("%d", &manga.volumes_list[i]);
        }
    }
    
    // Salvar alterações
    fseek(file, offset, SEEK_SET);
    fwrite(&manga, sizeof(Manga), 1, file);
    fclose(file);
    
    // Atualizar índice secundário se título mudou
    if (strcmp(old_title, manga.title) != 0) {
        remove_secondary_index(old_title);
        add_secondary_index(manga.title, manga.isbn);
        save_secondary_indices();
    }
    
    printf("Mangá atualizado com sucesso!\n");
}

// Deletar mangá
void delete_manga() {
    char search[MAX_TITLE];
    long offset;
    char *isbn;
    char confirm;
    
    printf("\n=== DELETAR MANGÁ ===\n");
    printf("Digite o ISBN ou título do mangá a ser deletado: ");
    getchar();
    fgets(search, MAX_TITLE, stdin);
    search[strcspn(search, "\n")] = 0;
    
    // Buscar mangá
    offset = find_manga_by_isbn(search);
    if (offset == -1) {
        isbn = find_isbn_by_title(search);
        if (isbn) {
            offset = find_manga_by_isbn(isbn);
        }
    }
    
    if (offset == -1) {
        printf("Mangá não encontrado!\n");
        return;
    }
    
    // Ler mangá
    FILE *file = fopen("mangas.dat", "r+b");
    if (!file) {
        printf("Erro ao abrir arquivo de dados!\n");
        return;
    }
    
    Manga manga;
    fseek(file, offset, SEEK_SET);
    fread(&manga, sizeof(Manga), 1, file);
    
    if (manga.deleted) {
        printf("Mangá já foi deletado!\n");
        fclose(file);
        return;
    }
    
    // Confirmação
    printf("Deseja realmente deletar o mangá '%s'? (s/N): ", manga.title);
    scanf(" %c", &confirm);
    
    if (confirm == 's' || confirm == 'S') {
        // Marcar como deletado
        manga.deleted = 1;
        fseek(file, offset, SEEK_SET);
        fwrite(&manga, sizeof(Manga), 1, file);
        
        // Remover dos índices
        remove_primary_index(manga.isbn);
        remove_secondary_index(manga.title);
        
        save_primary_indices();
        save_secondary_indices();
        
        printf("Mangá deletado com sucesso!\n");
    } else {
        printf("Operação cancelada.\n");
    }
    
    fclose(file);
}

// Listar todos os mangás
void list_all_mangas() {
    printf("\n=== LISTA DE MANGÁS ===\n");
    
    FILE *file = fopen("mangas.dat", "rb");
    if (!file) {
        printf("Nenhum mangá encontrado!\n");
        return;
    }
    
    Manga manga;
    int count = 0;
    
    while (fread(&manga, sizeof(Manga), 1, file)) {
        if (!manga.deleted) {
            printf("%d. %s (%s) - %d volumes adquiridos\n", 
                   ++count, manga.title, manga.isbn, manga.acquired_volumes);
        }
    }
    
    if (count == 0) {
        printf("Nenhum mangá ativo encontrado!\n");
    }
    
    fclose(file);
}

// Carregar dados iniciais do arquivo de texto
void load_initial_data() {
    FILE *file = fopen("mangas.txt", "r");
    if (!file) {
        printf("Arquivo mangas.txt não encontrado!\n");
        return;
    }
    
    char line[1000];
    Manga manga;
    
    while (fgets(line, sizeof(line), file)) {
        // Parse da linha
        char *token = strtok(line, ";");
        if (!token) continue;
        
        strcpy(manga.isbn, token);
        
        token = strtok(NULL, ";");
        if (!token) continue;
        strcpy(manga.title, token);
        
        token = strtok(NULL, ";");
        if (!token) continue;
        strcpy(manga.author, token);
        
        token = strtok(NULL, ";");
        if (!token) continue;
        manga.start_year = atoi(token);
        
        token = strtok(NULL, ";");
        if (!token) continue;
        if (strcmp(token, " -") == 0) {
            manga.end_year = -1;
        } else {
            manga.end_year = atoi(token);
        }
        
        token = strtok(NULL, ";");
        if (!token) continue;
        strcpy(manga.genre, token);
        
        token = strtok(NULL, ";");
        if (!token) continue;
        strcpy(manga.magazine, token);
        
        token = strtok(NULL, ";");
        if (!token) continue;
        strcpy(manga.publisher, token);
        
        token = strtok(NULL, ";");
        if (!token) continue;
        manga.edition_year = atoi(token);
        
        token = strtok(NULL, ";");
        if (!token) continue;
        manga.total_volumes = atoi(token);
        
        token = strtok(NULL, ";");
        if (!token) continue;
        manga.acquired_volumes = atoi(token);
        
        // Parse da lista de volumes
        token = strtok(NULL, ";");
        if (token) {
            char *volume_str = strtok(token, "[,]");
            int i = 0;
            while (volume_str && i < manga.acquired_volumes) {
                manga.volumes_list[i++] = atoi(volume_str);
                volume_str = strtok(NULL, "[,] ");
            }
        }
        
        manga.deleted = 0;
        
        // Verificar se já existe
        if (find_manga_by_isbn(manga.isbn) == -1) {
            // Salvar no arquivo
            FILE *data_file = fopen("mangas.dat", "ab");
            if (data_file) {
                long offset = ftell(data_file);
                fwrite(&manga, sizeof(Manga), 1, data_file);
                fclose(data_file);
                
                // Atualizar índices
                add_primary_index(manga.isbn, offset);
                add_secondary_index(manga.title, manga.isbn);
            }
        }
    }
    
    fclose(file);
    save_primary_indices();
    save_secondary_indices();
    printf("Dados iniciais carregados com sucesso!\n");
}

// Menu principal
void menu() {
    int option;
    
    do {
        printf("\n=== SISTEMA DE GERENCIAMENTO DE MANGÁS ===\n");
        printf("1. Criar mangá\n");
        printf("2. Buscar mangá\n");
        printf("3. Atualizar mangá\n");
        printf("4. Deletar mangá\n");
        printf("5. Listar todos os mangás\n");
        printf("6. Carregar dados iniciais\n");
        printf("7. Debug - Mostrar títulos indexados\n");
        printf("0. Sair\n");
        printf("Escolha uma opção: ");
        
        scanf("%d", &option);
        
        switch (option) {
            case 1:
                create_manga();
                break;
            case 2:
                read_manga();
                break;
            case 3:
                update_manga();
                break;
            case 4:
                delete_manga();
                break;
            case 5:
                list_all_mangas();
                break;
            case 6:
                load_initial_data();
                break;
            case 7:
                debug_titles();
                break;
            case 0:
                printf("Saindo...\n");
                break;
            default:
                printf("Opção inválida!\n");
        }
    } while (option != 0);
}

int main() {
    // Carregar índices existentes
    load_primary_indices();
    load_secondary_indices();
    
    printf("Sistema de Gerenciamento de Mangás iniciado!\n");
    printf("Índices carregados: %d primários, %d secundários\n", 
           primary_count, secondary_count);
    
    menu();
    
    // Liberar memória
    if (primary_indices) free(primary_indices);
    if (secondary_indices) free(secondary_indices);
    
    return 0;
}
