#define DEBUG_PARSE 0
struct haversine_pair
{
    r64 X0;
    r64 X1;
    r64 Y0;
    r64 Y1;
};

struct json_parser
{
    u64 JsonSize;
    u64 TokenCount;
    u64 PairCount;
    char *JsonMemory;
    struct memory_arena StringArena;
    struct memory_arena TokenArena;
    struct memory_arena PairArena;
    struct hashed_string *StringHash[256];
};

struct hashed_string
{
    struct hashed_string *NextInHash;
    // NOTE(chris): Stored after this is a null-terminated string
};

static inline u32
HashString(char *String, u32 Length)
{
    Assert(String);
    u32 Result = 0;
    while (Length-- && *String)
    {
        Result += Result*65521 + *String++;
    }
    return(Result);
}

static inline char *
CopyString(struct memory_arena *Arena, char *String, memory_size Length)
{
    char *Result = (char *)PushSize(Arena, 0);
    while (Length-- && *String)
    {
        *PushStruct(Arena, char) = *String++;
    }
    *PushStruct(Arena, char) = 0;
    return(Result);
}

static inline char *
GetStringFromHash(struct json_parser *Parser, char *String, memory_size Length)
{
    char *Result = 0;
    
    u32 HashValue = HashString(String, Length);
    u32 Index = HashValue & (ArrayCount(Parser->StringHash) - 1);
    Assert(Index < ArrayCount(Parser->StringHash));
    struct hashed_string *FirstInHash = Parser->StringHash[Index];

    for(struct hashed_string *Chain = FirstInHash;
        Chain;
        Chain = Chain->NextInHash)
    {
        char *HashedString = (char *)(Chain + 1);
        if(0 == strncmp(String, HashedString, Length))
        {
            Result = HashedString;
            break;
        }
    }

    if(!Result)
    {
        struct hashed_string *Header = PushStruct(&Parser->StringArena, struct hashed_string);
        Result = CopyString(&Parser->StringArena, String, Length);
        Header->NextInHash = FirstInHash;
        Parser->StringHash[Index] = Header;
    }

    return(Result);
}

struct char_stream
{
    u64 Size;
    u64 Position;
    char *Buffer;
};

struct token_stream
{
    u64 Size;
    u64 Position;
    struct json_token *Buffer;
};

enum json_token_type
{
    json_token_type_OpenBrace,
    json_token_type_CloseBrace,
    json_token_type_OpenBracket,
    json_token_type_CloseBracket,
    json_token_type_Comma,
    json_token_type_Colon,
    json_token_type_String,
    json_token_type_Number,
};

char *TOKEN_LOOKUP[] =
{
    "OpenBrace",
    "CloseBrace",
    "OpenBracket",
    "CloseBracket",
    "Comma",
    "Colon",
    "String",
    "Number",
};

u32 IsWhitespace(char Char)
{
    return Char == ' ' || Char == '\t' || Char == '\n' || Char == '\r';
}

char Advance(struct char_stream *Stream)
{
    char Result;
    if (Stream->Position < Stream->Size)
    {
        Result = Stream->Buffer[Stream->Position++];
    }
    else
    {
        Result = 0;
    }
    return Result;
}

char Peek(struct char_stream *Stream)
{
    char Result;
    if (Stream->Position < Stream->Size)
    {
        Result = Stream->Buffer[Stream->Position];
    }
    else
    {
        Result = 0;
    }
    return Result;
}

void SkipWhitespace(struct char_stream *Stream)
{
    while (IsWhitespace(Peek(Stream)))
    {
        Advance(Stream);
    }
}

struct json_token
{
    enum json_token_type Type;
    union
    {
        r64 Number;
        char *String;
    };
};

struct json_token *Emit(struct json_parser *Parser, enum json_token_type TokenType)
{
    Parser->TokenCount++;
    struct json_token *Token = PushStruct(&Parser->TokenArena, struct json_token);
    Token->Type = TokenType;
#if DEBUG_PARSE
    printf("%s\n", TOKEN_LOOKUP[Token->Type]);
#endif
    return Token;
}

struct json_token *EmitString(struct json_parser *Parser, char *String)
{
    struct json_token *Token = Emit(Parser, json_token_type_String);
    Token->String = String;
#if DEBUG_PARSE
    printf("%s: (%s)\n", TOKEN_LOOKUP[Token->Type], Token->String);
#endif
    return Token;
}

struct json_token *EmitNumber(struct json_parser *Parser, r64 Number)
{
    struct json_token *Token = Emit(Parser, json_token_type_Number);
    Token->Number = Number;
#if DEBUG_PARSE
    printf("%s: (%f)\n", TOKEN_LOOKUP[Token->Type], Token->Number);
#endif
    return Token;
}

void Tokenize(struct json_parser *Parser)
{
    struct char_stream _Stream =
    {
        .Size = Parser->JsonSize,
        .Position = 0,
        .Buffer = Parser->JsonMemory,
    };
    struct char_stream *Stream = &_Stream;
    u32 Continue = 1;
    while (Continue)
    {
        SkipWhitespace(Stream);
        char Char = Advance(Stream);
        switch (Char)
        {
            case '{':
            {
                Emit(Parser, json_token_type_OpenBrace);
            } break;

            case '}':
            {
                Emit(Parser, json_token_type_CloseBrace);
            } break;

            case '[':
            {
                Emit(Parser, json_token_type_OpenBracket);
            } break;

            case ']':
            {
                Emit(Parser, json_token_type_CloseBracket);
            } break;

            case ',':
            {
                Emit(Parser, json_token_type_Comma);
            } break;

            case ':':
            {
                Emit(Parser, json_token_type_Colon);
            } break;

            case '"':
            {
                u32 IsEscaped = 0;
                u32 StillParsing = 1;
                u64 Start = Stream->Position;
                do
                {
                    switch (Advance(Stream))
                    {
                        case 0:
                        {
                            StillParsing = 0;
                        } break;
                        
                        case '\\':
                        {
                            IsEscaped = !IsEscaped;
                        } break;

                        case '"':
                        {
                            StillParsing = IsEscaped;
                        } break;

                        default:
                        {
                            IsEscaped = 0;
                        } break;
                    }
                } while(StillParsing);
                u32 StringLength = Stream->Position - Start - 1;
                char *String = GetStringFromHash(Parser, Stream->Buffer + Start, StringLength);
                EmitString(Parser, String);
            } break;

            case 0:
            {
                Continue = 0;
            } break;

            default:
            {
                if ('0' <= Char && Char <= '9' || Char == '.' || Char == '-')
                {
                    r64 Number = 0;
                    r64 Decimal = 0;
                    b32 More = 0;
                    r64 Sign = 1;
                    do
                    {
                        if (Char == '-')
                        {
                            if (Sign < 1)
                            {
                                break;
                            }
                            Sign *= -1;
                        }
                        if (Char == '.')
                        {
                            if (Decimal)
                            {
                                break;
                            }
                            Decimal = 0.1;
                        }
                        else
                        {
                            if (Decimal)
                            {
                                Number += Decimal * (Char - '0');
                                Decimal *= 0.1;
                            }
                            else
                            {
                                Number = Number * 10.0 + Char - '0';
                            }
                        }
                        Char = Peek(Stream);
                        More = '0' <= Char && Char <= '9' || Char == '.';
                        if (More)
                        {
                            Advance(Stream);
                        }
                    } while (More);
                    EmitNumber(Parser, Sign * Number);
                }
                else
                {
                    printf("Unrecognized char '%c'\n", Char);
                    exit(1);
                }
            } break;
        }
    }
}

struct json_token Match(struct token_stream *Stream, enum json_token_type TokenType)
{
    struct json_token Result;
    if (Stream->Position < Stream->Size)
    {
        Result = Stream->Buffer[Stream->Position++];
        if (Result.Type != TokenType)
        {
            printf("Expected %s to be %s\n", TOKEN_LOOKUP[Result.Type], TOKEN_LOOKUP[TokenType]);
            exit(1);
        }
    }
    else
    {
        Assert(!"Reached the end of the token stream!");
    }
    return Result;
}

struct json_token MatchString(struct token_stream *Stream, char *String)
{
    struct json_token Result = Match(Stream, json_token_type_String);
    if(strcmp(Result.String, String))
    {
        printf("Expected '%s' to be '%s'\n", Result.String, String);
        exit(1);
    }
    return Result;
}

b32 Check(struct token_stream *Stream, enum json_token_type TokenType)
{
    b32 Result = Stream->Position < Stream->Size && Stream->Buffer[Stream->Position].Type == TokenType;
    return Result;
}

void Parse(struct json_parser *Parser)
{
    struct token_stream Stream_ =
    {
        .Size = Parser->TokenCount,
        .Position = 0,
        .Buffer = Parser->TokenArena.Memory,
    };
    struct token_stream *Stream = &Stream_;
    Match(Stream, json_token_type_OpenBrace);
    MatchString(Stream, "pairs");
    Match(Stream, json_token_type_Colon);
    Match(Stream, json_token_type_OpenBracket);
    b32 More = Check(Stream, json_token_type_OpenBrace);
    while (More)
    {
        ++Parser->PairCount;
        struct haversine_pair *Pair = PushStruct(&Parser->PairArena, struct haversine_pair);
        Match(Stream, json_token_type_OpenBrace);
        MatchString(Stream, "x0");
        Match(Stream, json_token_type_Colon);
        Pair->X0 = Match(Stream, json_token_type_Number).Number;
        Match(Stream, json_token_type_Comma);
        MatchString(Stream, "y0");
        Match(Stream, json_token_type_Colon);
        Pair->Y0 = Match(Stream, json_token_type_Number).Number;
        Match(Stream, json_token_type_Comma);
        MatchString(Stream, "x1");
        Match(Stream, json_token_type_Colon);
        Pair->X1 = Match(Stream, json_token_type_Number).Number;
        Match(Stream, json_token_type_Comma);
        MatchString(Stream, "y1");
        Match(Stream, json_token_type_Colon);
        Pair->Y1 = Match(Stream, json_token_type_Number).Number;
        Match(Stream, json_token_type_CloseBrace);
        More = Check(Stream, json_token_type_Comma);
        if (More)
        {
            Match(Stream, json_token_type_Comma);
        }
    }
    Match(Stream, json_token_type_CloseBracket);
    Match(Stream, json_token_type_CloseBrace);
}

u64 ParseHaversinePairs(struct memory_arena JsonArena, struct memory_arena *Arena, struct haversine_pair **Pairs)
{
    struct json_parser Parser = {0};
    Parser.JsonSize = JsonArena.Size;
    Parser.JsonMemory = JsonArena.Memory;
    Parser.StringArena = SubArena(Arena, 1024);
    Parser.TokenArena = SubArena(Arena, Arena->Size - Arena->Used);
    Tokenize(&Parser);
    Parser.PairArena = SubArena(&Parser.TokenArena, Parser.TokenArena.Size - Parser.TokenArena.Used);
    Parse(&Parser);
    *Pairs = (struct haversine_pair *)Parser.PairArena.Memory;
    return Parser.PairCount;
}
