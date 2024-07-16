struct char_stream
{
    u64 Size;
    u64 Position;
    char *Buffer;
};

enum json_token
{
    json_token_OpenBrace,
    json_token_CloseBrace,
    json_token_OpenBracket,
    json_token_CloseBracket,
    json_token_Comma,
    json_token_Colon,
    json_token_String,
    json_token_Number,
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

struct token_stream_builder
{
    u32 Count;
    u64 StringPoolSize;
};

void Emit(struct token_stream_builder *Builder, enum json_token Token)
{
    Builder->Count++;
}

struct token_stream_builder Tokenize(u64 Size, char *Buffer)
{
    struct char_stream _Stream =
        {
            .Size = Size,
            .Position = 0,
            .Buffer = Buffer,
        };
    struct char_stream *Stream = &_Stream;
    struct token_stream_builder Builder = {0};
    u32 Continue = 1;
    while (Continue)
    {
        SkipWhitespace(Stream);
        char Char = Advance(Stream);
        switch (Char)
        {
            case '{':
            {
                Emit(&Builder, json_token_OpenBrace);
            } break;

            case '}':
            {
                Emit(&Builder, json_token_CloseBrace);
            } break;

            case '[':
            {
                Emit(&Builder, json_token_OpenBracket);
            } break;

            case ']':
            {
                Emit(&Builder, json_token_CloseBracket);
            } break;

            case ',':
            {
                Emit(&Builder, json_token_Comma);
            } break;

            case ':':
            {
                Emit(&Builder, json_token_Colon);
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
                Builder.StringPoolSize += (Stream->Position - Start);
                Emit(&Builder, json_token_String);
            } break;

            case 0:
            {
                Continue = 0;
            } break;

            default:
            {
                if ('0' <= Char && Char <= '9')
                {
                    u32 Number = 0;
                    do
                    {
                        Number = Number * 10 + Char - '0';
                        Char = Advance(Stream);
                    } while ('0' <= Char && Char <= '9');
                    Emit(&Builder, json_token_Number);
                }
            } break;
        }
    }
    return Builder;
}
