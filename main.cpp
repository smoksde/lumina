#include "lumina.hpp"
#include <vector>
#include "agent.hpp"
#include "energy.hpp"
#include "render_buffer.hpp"

int main()
{   
    const char* title = "lumina";

    const int fps = 60;
    const int tps = 1;

    int frame_width = 1400;
    int frame_height = 1400;
    Viewport viewport = Viewport(title, frame_width, frame_height);

    Shader agent_shader("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");
    Shader energy_shader("shaders/energy.vs", "shaders/energy.fs");

    const int num_agents = 12;
    
    std::vector<Agent> agent_list;
    std::vector<Energy> energy_list;

    for (int i = 0; i < num_agents; i++)
    {
        agent_list.push_back(Agent());
    }

    std::vector<glm::vec3> agent_positions;
    agent_positions.reserve(num_agents);
    std::vector<float> agent_rotations;
    agent_rotations.reserve(num_agents);

    std::vector<glm::vec3> energy_positions;

    // TODO: Vertices should be outsourced

    float quad_vertices[] =
    {
        -1.0f, -1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,

        -1.0f, -1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
    };

    float triangle_vertices[] =
    {
        -1.0f, 1.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
    };

    RenderBuffer agent_render_buffer = RenderBuffer();
    agent_render_buffer.setVertexData(triangle_vertices, sizeof(triangle_vertices));
    agent_render_buffer.setVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    RenderBuffer energy_render_buffer = RenderBuffer();
    energy_render_buffer.setVertexData(quad_vertices, sizeof(quad_vertices));
    energy_render_buffer.setVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glm::mat4 view = glm::mat4(1.0f); // Identity matrix for simplicity
    glm::mat4 projection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f); // Orthographic projection

    GLint viewLoc = glGetUniformLocation(agent_shader.shader_id, "view");
    GLint projLoc = glGetUniformLocation(agent_shader.shader_id, "projection");

    bool running = true;
    SDL_Event event;

    glPointSize(5.0f);

    uint64 sdl_ticks = SDL_GetTicks64();
    uint64 last_tick_time = sdl_ticks;
    uint64 last_update_time = sdl_ticks;
    uint64 last_render_time = sdl_ticks;
    uint64 current_time = sdl_ticks;

    while (running)
    {
        current_time = SDL_GetTicks64();

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }
        }

        float tick_delta_time = current_time - last_tick_time;
        float update_delta_time = current_time - last_update_time;
        float render_delta_time = current_time - last_render_time;

        if (tick_delta_time >= 1000 / tps)
        {
            for (int i = 0; i < 10; i++)
            {
                energy_list.push_back(Energy());
            }

            last_tick_time = current_time;
        }

        // Update

        for (auto& agent : agent_list)
        {
            agent.update(update_delta_time, energy_list);
        }

        for (auto& energy : energy_list)
        {
            energy.update(update_delta_time);
        }

        last_update_time = current_time;

        if (render_delta_time >= 1000 / fps)
        {
            agent_positions.clear();
            agent_rotations.clear();
            for (auto& agent : agent_list)
            {
                agent_positions.push_back(agent.position);
                agent_rotations.push_back(agent.rotation);
            }

            energy_positions.clear();
            for (auto& energy : energy_list)
            {
                energy_positions.push_back(energy.position);
            }

            glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
            glClear(GL_COLOR_BUFFER_BIT);

            // Render agents
            agent_render_buffer.bind();
            agent_render_buffer.setInstanceData(num_agents, sizeof(glm::vec3) * num_agents + sizeof(float) * num_agents);
            agent_render_buffer.setInstanceSubData(0, sizeof(glm::vec3) * num_agents, agent_positions.data());
            agent_render_buffer.setInstanceSubData(sizeof(glm::vec3) * num_agents, sizeof(float) * num_agents, agent_rotations.data());
            agent_render_buffer.setInstanceAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0, 1);
            agent_render_buffer.setInstanceAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)(sizeof(glm::vec3) * num_agents), 1);

            agent_shader.bind();
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

            
            glDrawArraysInstanced(GL_TRIANGLES, 0, 3, num_agents);
            agent_render_buffer.unbind();

            // Render energies
            energy_render_buffer.bind();
            
            int num_energies = energy_list.size();
            energy_render_buffer.setInstanceData(num_energies, sizeof(glm::vec3) * num_energies);
            energy_render_buffer.setInstanceSubData(0, sizeof(glm::vec3) * num_energies, energy_positions.data());
            energy_render_buffer.setInstanceAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0, 1);

            energy_shader.bind();
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

            
            glDrawArraysInstanced(GL_TRIANGLES, 0, 6, num_energies);
            energy_render_buffer.unbind();

            SDL_GL_SwapWindow(viewport.window);
            last_render_time = current_time;
        }
    }

    agent_render_buffer.~RenderBuffer();

    return 0;
}